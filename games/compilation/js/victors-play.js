/* Ultimate Victors play + credits (dog caption locked). */
(function () {
  const CAPTION = "Viber Coder: Ryan Fisher";

  const tables = [
    { id: "btm.jolly_joker", name: "Jolly Joker", game: "Behind the Mask", year: 1993 },
    { id: "btm.blackbeard", name: "Blackbeard and Ironmen", game: "Behind the Mask", year: 1993 },
    { id: "btm.wizard", name: "Wizard", game: "Behind the Mask", year: 1993 },
    { id: "sp2.space_sister", name: "Space Sister", game: "Super Pinball II", year: 1995 },
    { id: "sp2.spy_eyes", name: "The Spy Eyes", game: "Super Pinball II", year: 1995 },
    { id: "sp2.show_time", name: "Show Time", game: "Super Pinball II", year: 1995 },
    { id: "lg.glad", name: "Gladiators", game: "Last Gladiators", year: 1995 },
    { id: "lg.rose", name: "Knight of the Roses", game: "Last Gladiators", year: 1995 },
    { id: "lg.drago", name: "Dragon Showdown", game: "Last Gladiators", year: 1995 },
    { id: "lg.war", name: "Warlock", game: "Last Gladiators", year: 1995 },
    { id: "lg.victors", name: "Victors", game: "Last Gladiators", year: 1995, alt: "lg.ultimate_victors" },
    { id: "necro.arkham", name: "Arkham", game: "Necronomicon", year: 1996 },
    { id: "necro.bloody", name: "Cult of the Bloody Tongue", game: "Necronomicon", year: 1996 },
    { id: "necro.dreamlands", name: "Dreamlands", game: "Necronomicon", year: 1996 }
  ];

  const OPT_KEY = "kaze.compilation.options.v1";
  function loadOptions() {
    try {
      return JSON.parse(localStorage.getItem(OPT_KEY) || "{}");
    } catch (_) {
      return {};
    }
  }
  function saveOptions(o) {
    localStorage.setItem(OPT_KEY, JSON.stringify(o));
  }

  let selected = 10;
  let uvUnlocked = false;
  let harness = null;
  let raf = 0;

  function $(sel) { return document.querySelector(sel); }

  function renderList() {
    const ul = $("#table-list");
    ul.innerHTML = "";
    tables.forEach((t, i) => {
      const li = document.createElement("li");
      let label = t.name;
      if (t.id === "lg.victors" && uvUnlocked) label = "Ultimate Victors";
      li.innerHTML = `<span>${label}</span><span class="meta">${t.game}</span>`;
      if (i === selected) li.classList.add("active");
      li.addEventListener("click", () => { selected = i; renderList(); });
      ul.appendChild(li);
    });
  }

  function applyPortraitClass(on) {
    document.documentElement.classList.toggle("portrait-res", !!on);
    document.body.classList.toggle("portrait-res", !!on);
  }

  function showCredits() {
    $("#menu-panel").style.display = "none";
    $("#play-panel").style.display = "none";
    const c = $("#credits-stage");
    c.classList.add("on");
    $("#credits-caption").textContent = CAPTION;
  }

  function showMenu() {
    $("#credits-stage").classList.remove("on");
    $("#play-panel").style.display = "none";
    $("#menu-panel").style.display = "block";
  }

  async function startVictors() {
    $("#menu-panel").style.display = "none";
    $("#credits-stage").classList.remove("on");
    $("#play-panel").style.display = "block";
    $("#hud-score").textContent = "SCORE 0";
    $("#hud-title").textContent = uvUnlocked ? "ULTIMATE VICTORS" : "VICTORS";

    if (!harness) harness = await LgPhysicsHarness.createHarness();
    const world = harness.world;
    world.ball.x = 0x2000;
    world.ball.y = 0x2000;
    world.ball.vel_x = 0x40;
    world.ball.vel_y = 0;

    const canvas = $("#play-canvas");
    const ctx = canvas.getContext("2d");
    const W = canvas.width, H = canvas.height;

    function draw() {
      harness.tick(0, 0);
      ctx.fillStyle = "#0c141c";
      ctx.fillRect(0, 0, W, H);
      /* Playfield frame */
      ctx.strokeStyle = "#3ad0c8";
      ctx.strokeRect(8, 8, W - 16, H - 16);
      /* Flippers — shell pivots mapped into canvas */
      const sx = (x) => ((x - 0x0800) / 0x3000) * W;
      const sy = (y) => ((y - 0x1000) / 0x7000) * H;
      ctx.fillStyle = "#f0b43a";
      ctx.fillRect(sx(world.flip_lx) - 18, sy(world.flip_ly), 36, 8);
      ctx.fillRect(sx(world.flip_rx) - 18, sy(world.flip_ry), 36, 8);
      /* Ball */
      ctx.beginPath();
      ctx.arc(sx(world.ball.x), sy(world.ball.y), 5, 0, Math.PI * 2);
      ctx.fillStyle = "#e8eef6";
      ctx.fill();
      raf = requestAnimationFrame(draw);
    }
    cancelAnimationFrame(raf);
    draw();
  }

  function wireKonami() {
    const seq = ["ArrowUp","ArrowUp","ArrowDown","ArrowDown","ArrowLeft","ArrowRight","ArrowLeft","ArrowRight","KeyB","KeyA","Enter"];
    let i = 0;
    window.addEventListener("keydown", (e) => {
      if (e.code === seq[i]) {
        i++;
        if (i === seq.length) {
          uvUnlocked = true;
          i = 0;
          renderList();
          const clip = $("#unlock-note");
          clip.textContent = "It's Dark Knowledge, beware. — Ultimate Victors unlocked";
        }
      } else {
        i = e.code === seq[0] ? 1 : 0;
      }
      if (e.code === "ArrowRight" && tables[selected].id === "lg.victors") {
        uvUnlocked = true;
        renderList();
      }
      if (e.code === "Digit4") {
        const o = loadOptions();
        o.portrait_resolution = !o.portrait_resolution;
        saveOptions(o);
        applyPortraitClass(o.portrait_resolution);
      }
    });
  }

  document.addEventListener("DOMContentLoaded", async () => {
    const o = loadOptions();
    if (o.physics_pack_global == null) o.physics_pack_global = "lg-97";
    if (o.portrait_resolution == null) o.portrait_resolution = false;
    saveOptions(o);
    applyPortraitClass(o.portrait_resolution);

    renderList();
    wireKonami();
    $("#btn-play").addEventListener("click", startVictors);
    $("#btn-credits").addEventListener("click", showCredits);
    $("#btn-menu").addEventListener("click", () => {
      cancelAnimationFrame(raf);
      showMenu();
    });
    $("#btn-credits-back").addEventListener("click", showMenu);

    $("#credits-caption").textContent = CAPTION;
  });

  /* Export for smoke */
  globalThis.UV_CREDITS_CAPTION = CAPTION;
  globalThis.KAZE_TABLES = tables;
})();
