/* UV loader for native WASM when present; falls back to lg-physics.js. */
(function (global) {
  const assetBase = (global.KAZE_ASSET_BASE || "assets/physics/");

  async function tryLoadWasm() {
    const url = assetBase + "lg_phys.wasm";
    try {
      const res = await fetch(url);
      if (!res.ok) return null;
      const buf = await res.arrayBuffer();
      /* Full WASI glue omitted — host rebuilds wasm via wasi-sdk when available. */
      return { bytes: buf.byteLength, url };
    } catch (_) {
      return null;
    }
  }

  async function loadSondCatalog() {
    try {
      const res = await fetch(assetBase + "sfx/sond_catalog.json");
      if (!res.ok) return null;
      return await res.json();
    } catch (_) {
      return null;
    }
  }

  /** r5 priority → gain_scale = r5/5; per-voice 0.55*scale/√n (DP-28). */
  function voiceGain(r5, nVoices) {
    const scale = (r5 || 5) / 5;
    const n = Math.max(1, nVoices || 1);
    return (0.55 * scale) / Math.sqrt(n);
  }

  async function createHarness() {
    const wasm = await tryLoadWasm();
    const sond = await loadSondCatalog();
    const world = global.LgPhysics.createWorld();
    return {
      mode: wasm ? "c_wasm" : "js_fallback",
      wasm,
      sond,
      world,
      voiceGain,
      bindMask(bytes) {
        return global.LgPhysics.loadSegsBE(world, bytes);
      },
      tick(l, r) {
        global.LgPhysics.tick(world, l, r);
      }
    };
  }

  global.LgPhysicsHarness = { createHarness, voiceGain, assetBase };
})(typeof globalThis !== "undefined" ? globalThis : window);
