/* Dump-free LG Victors physics JS port (DP-18..DP-30). Default pack lg-97. */
(function (global) {
  const FLIP_L_X = 0x1400, FLIP_R_X = 0x2c00, FLIP_Y = 0x6800;
  const E_SOFT = 0x012c, E_FLIP = 0x01d6;
  const CASE3_BOOST = 0x3a20;
  const DEFAULT_FLAGS = 0x0100;

  function contactFlagsFromCmd(cmd) {
    const op = cmd & 0x0f00;
    if (op === 0x0100) {
      const lo = cmd & 0x000f;
      let base = 0;
      if (lo === 2) base = 0x1000;
      else if (lo === 3) base = 0x3000;
      else if (lo === 4) base = 0x2000;
      const hi = cmd & 0x8000 ? 0x8100 : 0x0100;
      return (base + hi) & 0xffff;
    }
    if (op === 0x0700) return (0x6000 | cmd) & 0xffff;
    return DEFAULT_FLAGS;
  }

  function bounceType(flags) {
    const t = (flags >> 8) & 15;
    if ([2, 3, 5, 6, 7, 8, 10].includes(t)) return t;
    return 2;
  }

  function createWorld() {
    return {
      segs: [],
      ball: {
        x: 0x2000, y: 0x1800, vel_x: 0, vel_y: 0,
        basis_c: 16384, basis_s: 0, grav_y: 0xf0,
        contact_flags: 0, hit: 0
      },
      flip_lx: FLIP_L_X, flip_ly: FLIP_Y,
      flip_rx: FLIP_R_X, flip_ry: FLIP_Y,
      case8: 0, sfx_r4: 0, sfx_r5: 0,
      pack: "lg-97"
    };
  }

  function loadSegsBE(world, bytes) {
    world.segs = [];
    const n = Math.floor(bytes.length / 12);
    const view = bytes instanceof Uint8Array ? bytes : new Uint8Array(bytes);
    for (let i = 0; i < n; i++) {
      const o = i * 12;
      const be = (a) => (view[o + a] << 8) | view[o + a + 1];
      const seg = { w0: be(0), w1: be(2), w2: be(4), w3: be(6), w4: be(8), w5: be(10) };
      if (!(seg.w0 | seg.w1 | seg.w2 | seg.w3 | seg.w4 | seg.w5)) continue;
      seg.contact_flags = contactFlagsFromCmd(0x0100);
      world.segs.push(seg);
    }
    return world.segs.length;
  }

  function probe(world, x, y) {
    for (let i = 0; i < world.segs.length; i++) {
      const s = world.segs[i];
      const mx = (s.w0 + s.w2) / 2;
      const my = (s.w1 + s.w3) / 2;
      let r = s.w5 & 0xff;
      if (r > 32) r = 32;
      if (r < 1) r = 8;
      const dx = x - mx, dy = y - my;
      if (dx * dx + dy * dy <= r * r) return i;
    }
    return -1;
  }

  function bounceResponse(n, e, boost) {
    if (n < 0) n = -((n * e) >> 10);
    return n + (boost || 0);
  }

  function collide(world) {
    const o = world.ball;
    const i = probe(world, o.x, o.y);
    o.hit = i >= 0 ? 1 : 0;
    world.case8 = 0;
    world.sfx_r4 = 0;
    world.sfx_r5 = 0;
    if (i < 0) return;
    const flags = world.segs[i].contact_flags;
    o.contact_flags = flags;
    const typ = bounceType(flags);
    let n = o.vel_y; /* coarse host normal */
    let e = E_SOFT, boost = 0;
    if (typ === 3) { e = 0; boost = CASE3_BOOST; }
    else if (typ === 5) { e = 0; boost = 0x10; }
    else if (typ === 6) { e = E_FLIP; }
    else if (typ === 7) { n -= n >> 3; }
    else if (typ === 8) {
      e = 0;
      world.case8 = 1;
      world.sfx_r4 = 2;
      world.sfx_r5 = 5;
    } else if (typ === 10) {
      world.sfx_r4 = 2;
      world.sfx_r5 = 1;
    }
    o.vel_y = bounceResponse(n, e, boost);
  }

  function tick(world, left, right) {
    world.flip_lx = FLIP_L_X;
    world.flip_rx = FLIP_R_X;
    world.flip_ly = world.flip_ry = FLIP_Y;
    void left; void right;
    world.ball.vel_y -= world.ball.grav_y;
    world.ball.x += world.ball.vel_x >> 3;
    world.ball.y += world.ball.vel_y >> 3;
    collide(world);
  }

  global.LgPhysics = {
    FLIP_L_X, FLIP_R_X, FLIP_Y, DEFAULT_FLAGS,
    contactFlagsFromCmd, bounceType, createWorld, loadSegsBE, probe, tick, collide
  };
})(typeof globalThis !== "undefined" ? globalThis : window);
