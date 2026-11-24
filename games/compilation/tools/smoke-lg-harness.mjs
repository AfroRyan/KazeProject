#!/usr/bin/env node
/** Smoke DP-30 binder + flipper shell + credits caption (no ISO required). */
import { readFileSync, existsSync } from "node:fs";
import { dirname, join } from "node:path";
import { fileURLToPath } from "node:url";
import vm from "node:vm";

const root = join(dirname(fileURLToPath(import.meta.url)), "..");
let fails = 0;
function check(cond, msg) {
  if (!cond) {
    console.error("FAIL:", msg);
    fails++;
  } else {
    console.log("ok:", msg);
  }
}

function loadScript(rel) {
  const code = readFileSync(join(root, rel), "utf8");
  const sandbox = { console, globalThis: {} };
  sandbox.globalThis = sandbox;
  sandbox.window = sandbox;
  vm.runInNewContext(code, sandbox, { filename: rel });
  return sandbox;
}

const phys = loadScript("js/lg-physics.js");
const LP = phys.LgPhysics || phys.globalThis?.LgPhysics;
check(!!LP, "LgPhysics loaded");

check(LP.contactFlagsFromCmd(0x0101) === 0x0100, "binder 0x0101 → 0x0100");
check(LP.bounceType(0x0100) === 2, "default flags → soft 2");
check(LP.contactFlagsFromCmd(0x0102) === 0x1100, "binder 0x0102 → 0x1100");
check(LP.contactFlagsFromCmd(0x0705) === 0x6705, "binder 0x0705 → 0x6705");
check(LP.FLIP_L_X === 0x1400 && LP.FLIP_R_X === 0x2c00 && LP.FLIP_Y === 0x6800,
  "flipper shell 0x1400/0x2c00×0x6800");

const world = LP.createWorld();
const seg = new Uint8Array(12);
const be = (o, v) => { seg[o] = v >> 8; seg[o + 1] = v & 0xff; };
be(0, 0x1f00); be(2, 0x2f00); be(4, 0x2100); be(6, 0x3100); be(8, 0x2200); be(10, 0x0010);
check(LP.loadSegsBE(world, seg) === 1, "load 1 seg");
check(world.segs[0].contact_flags === 0x0100, "seg contact_flags default (not word4)");
check(world.segs[0].w4 === 0x2200, "word4 residual retained");

world.ball.x = 0x2000;
world.ball.y = 0x3000;
world.ball.vel_y = -0x200;
LP.collide(world);
check(world.ball.hit === 1, "collide hit");
check(world.ball.contact_flags === 0x0100, "collide used contact_flags");

/* Credits caption + assets */
const html = readFileSync(join(root, "index.html"), "utf8");
check(html.includes("Viber Coder: Ryan Fisher"), "HTML credits caption");
check(existsSync(join(root, "assets/dmd/ultimate-victors-dog-dmd.png")), "dog DMD asset");
check(existsSync(join(root, "assets/credits/ultimate-victors-dog-credits-crop.png")),
  "dog credits crop");

/* victors-play expects document — caption checked via source only */
const playSrc = readFileSync(join(root, "js/victors-play.js"), "utf8");
check(playSrc.includes("Viber Coder: Ryan Fisher"), "victors-play caption locked");

/* Optional poly / segs from ISO extract */
const segsPath = join(root, "assets/physics/victor_MASK_segs.bin");
const polyPath = join(root, "assets/physics/victor_MASK_poly.bin");
if (existsSync(segsPath)) {
  const n = LP.loadSegsBE(world, readFileSync(segsPath));
  /* TOC MASK..BACK yields 659 (older notes said 653). */
  check(n === 659 || n === 653, `ISO segs count ${n} in {659,653}`);
  check(world.segs[0].contact_flags === 0x0100, "ISO segs default contact_flags");
} else {
  console.log("skip: victor_MASK_segs.bin (needs ISO extract)");
}
if (existsSync(polyPath)) {
  console.log("note: victor_MASK_poly.bin present (native harness parses poly)");
} else {
  console.log("skip: victor_MASK_poly.bin (needs ISO extract)");
}

/* SOND rates documented when catalog present */
const cat = join(root, "assets/physics/sfx/sond_catalog.json");
if (existsSync(cat)) {
  const j = JSON.parse(readFileSync(cat, "utf8"));
  const rates = (j.primary || j.rings || []).map((r) => r.rate_hz || r.rate);
  if (rates.length >= 3) {
    check(rates[0] === 2950 && rates[1] === 2939 && rates[2] === 2966, "SOND OCT/FNS rates");
  }
} else {
  console.log("skip: sond_catalog.json (needs ISO extract)");
}

if (fails) {
  console.log("RESULT FAIL", fails);
  process.exit(1);
}
console.log("RESULT PASS");
