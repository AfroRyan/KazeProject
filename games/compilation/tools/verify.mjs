#!/usr/bin/env node
import { existsSync } from "node:fs";
import { dirname, join } from "node:path";
import { fileURLToPath } from "node:url";
import { spawnSync } from "node:child_process";

const root = join(dirname(fileURLToPath(import.meta.url)), "..");
const repo = join(root, "../..");

const required = [
  "index.html",
  "js/lg-physics.js",
  "js/lg-physics-harness.js",
  "js/victors-play.js",
  "css/shell.css",
  "assets/dmd/ultimate-victors-dog-dmd.png",
  "assets/credits/ultimate-victors-dog-credits-crop.png",
  "assets/physics/victor_FLIP_pivots.json",
  "data/lg-physics-notes.json"
];

let fails = 0;
for (const r of required) {
  if (!existsSync(join(root, r))) {
    console.error("missing", r);
    fails++;
  }
}

const harness = spawnSync("make", ["-C", join(repo, "games/last-gladiators/harness"), "run"], {
  encoding: "utf8"
});
if (harness.status !== 0) {
  console.error(harness.stdout, harness.stderr);
  fails++;
} else {
  console.log(harness.stdout.trim());
}

const smoke = spawnSync("node", [join(root, "tools/smoke-lg-harness.mjs")], { encoding: "utf8" });
process.stdout.write(smoke.stdout);
process.stderr.write(smoke.stderr || "");
if (smoke.status !== 0) fails++;

if (fails) {
  console.log("VERIFY FAIL");
  process.exit(1);
}
console.log("VERIFY OK");
