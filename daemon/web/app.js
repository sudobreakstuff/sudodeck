const iv = new IntersectionObserver(function(es) {
  es.forEach(function(e) { if (e.isIntersecting) e.target.classList.add('in'); });
}, { threshold: 0.1 });
document.querySelectorAll('.rv').forEach(function(el) { iv.observe(el); });
document.querySelectorAll('.section-reveal').forEach(function(el) { iv.observe(el); });

var dp = null, dr = null, dw = null, cd = null, cs = 0, sp = 0, sb = -1, __srl_buf = '', __busy = false;

function dc() {
  return {
    version: 2, name: "SudoDeck", grid: { cols: 4, rows: 3 },
    wifi: { ssid: "", password: "" },
    saver: { timeout: 30, sleep: 60, mode: 0 },
    theme: { name: "default", button_style: "flat" },
    widgets: [],
    automations: [],
    pages: [
      { name: "Main", buttons: [
        { label: "Copy", color: "#16213E", action: { type: "combo", mod: "CTRL", key: "c" } },
        { label: "Paste", color: "#16213E", action: { type: "combo", mod: "CTRL", key: "v" } },
        { label: "Cut", color: "#16213E", action: { type: "combo", mod: "CTRL", key: "x" } },
        { label: "Undo", color: "#16213E", action: { type: "combo", mod: "CTRL", key: "z" } },
        { label: "Save", color: "#1A3A1A", action: { type: "combo", mod: "CTRL", key: "s" } },
        { label: "Find", color: "#16213E", action: { type: "combo", mod: "CTRL", key: "f" } },
        { label: "SelAll", color: "#16213E", action: { type: "combo", mod: "CTRL", key: "a" } },
        { label: "Tab", color: "#16213E", action: { type: "key", value: "TAB" } },
        { label: "Enter", color: "#1A2A1A", action: { type: "key", value: "ENTER" } },
        { label: "Esc", color: "#3A1A1A", action: { type: "key", value: "ESC" } },
        { label: "Vol+", color: "#1A1A3A", action: { type: "key", value: "MEDIA_VOLUME_UP" } },
        { label: "Vol-", color: "#1A1A3A", action: { type: "key", value: "MEDIA_VOLUME_DOWN" } }
      ]},
      { name: "Media", buttons: [
        { label: "Play", color: "#1A2A1A", action: { type: "key", value: "MEDIA_PLAY_PAUSE" } },
        { label: "Next", color: "#16213E", action: { type: "key", value: "MEDIA_NEXT_TRACK" } },
        { label: "Prev", color: "#16213E", action: { type: "key", value: "MEDIA_PREV_TRACK" } },
        { label: "Mute", color: "#3A1A1A", action: { type: "key", value: "MEDIA_MUTE" } },
        { label: "Br+", color: "#1A1A3A", action: { type: "key", value: "BRIGHTNESS_UP" } },
        { label: "Br-", color: "#1A1A3A", action: { type: "key", value: "BRIGHTNESS_DOWN" } },
        { label: "Home", color: "#16213E", action: { type: "key", value: "HOME" } },
        { label: "End", color: "#16213E", action: { type: "key", value: "END" } },
        { label: "PgUp", color: "#16213E", action: { type: "key", value: "PAGE_UP" } },
        { label: "PgDn", color: "#16213E", action: { type: "key", value: "PAGE_DOWN" } },
        { label: "Screensht", color: "#3A2A1A", action: { type: "combo", mod: "CTRL_SHIFT", key: "s" } },
        { label: "Lock", color: "#3A1A1A", action: { type: "combo", mod: "CTRL_ALT", key: "l" } }
      ]},
      { name: "Studio", buttons: [
        { label: "Split", color: "#16213E", action: { type: "combo", mod: "CTRL", key: "k" } },
        { label: "Undo", color: "#16213E", action: { type: "combo", mod: "CTRL", key: "z" } },
        { label: "Redo", color: "#16213E", action: { type: "combo", mod: "CTRL", key: "y" } },
        { label: "Snip", color: "#3A2A1A", action: { type: "combo", mod: "CTRL", key: "t" } },
        { label: "FullScr", color: "#16213E", action: { type: "key", value: "F11" } },
        { label: "TmLine", color: "#1A2A1A", action: { type: "key", value: "F3" } },
        { label: "Render", color: "#3A1A1A", action: { type: "key", value: "F8" } },
        { label: "NewPrj", color: "#16213E", action: { type: "combo", mod: "CTRL", key: "n" } },
        { label: "Export", color: "#1A2A1A", action: { type: "combo", mod: "CTRL", key: "m" } },
        { label: "Import", color: "#16213E", action: { type: "combo", mod: "CTRL", key: "i" } },
        { label: "DelRpl", color: "#3A1A1A", action: { type: "combo", mod: "CTRL_SHIFT", key: "d" } },
        { label: "AddTX", color: "#1A1A3A", action: { type: "combo", mod: "CTRL", key: "t" } }
      ]}
    ]
  };
}
function curPage() { return cd && cd.pages && sp < cd.pages.length ? cd.pages[sp] : null; }
function curBtn() { var p = curPage(); return p && sb >= 0 && sb < p.buttons.length ? p.buttons[sb] : null; }

function ic() { cd = dc(); }
function tm(m, t) { var o = document.createElement('div'); o.className = 'toast ' + (t||'ok'); o.textContent = m; document.body.appendChild(o); setTimeout(function() { o.remove(); }, 3000); }
function ss(on) {
  var b = document.getElementById('sb');
  b.className = 'badge ' + (on ? 'on' : 'off');
  b.querySelector('.dot').className = 'dot ' + (on ? 'g' : 'r');
  b.querySelector('span').textContent = on ? 'CONNECTED' : 'DISCONNECTED';
}
async function srl(t) {
  t = t || 5000; var d = new TextDecoder(), b = __srl_buf, s = Date.now(); __srl_buf = '';
  while (Date.now() - s < t) {
    try { var v = await dr.read(); if (v.done) break; if(!v.value||!v.value.length) continue; b += d.decode(v.value, {stream:true}); var n = b.indexOf('\n'); if(n>=0) { var l = b.substring(0,n); var j = l.indexOf('{'); if(j>=0) { __srl_buf = b.substring(n+1); return l.substring(j).trim(); } } }
    catch(e) { if(e.message&&e.message.indexOf('device has been lost')>=0) { ss(false); dr=null; dw=null; throw new Error('lost'); } await new Promise(function(r) { setTimeout(r, 50); }); }
  }
  var j = b.indexOf('{'); return j>=0 ? b.substring(j).trim() : '';
}
async function sc(c, t) { if (!dw) throw new Error('nc'); if(!dr) throw new Error('nr'); await dw.write(new TextEncoder().encode(JSON.stringify(c)+'\n')); await new Promise(function(r) { setTimeout(r, 200); }); var s = await srl(t); if (!s) throw new Error('no response'); return JSON.parse(s); }

// Connect via serial
document.getElementById('btnConnect').addEventListener('click', async function() {
  try {
    if (!('serial' in navigator)) { tm('Web Serial needs Chrome/Edge', 'ng'); return; }
    tryReleaseDaemon();
    var p = await navigator.serial.requestPort(); await p.open({baudRate:115200});
    dp = p; dr = p.readable.getReader(); dw = p.writable.getWriter(); ss(true); tm('Connected');
    // Retry get_info with delays — port open can reset ESP32 (DTR/RTS toggle)
    for (var _i = 0; _i < 5; _i++) {
      try {
        __srl_buf = '';
        var r = await sc({cmd:'get_info'}, 3000);
        if (r && r.name) { tm(r.name+' v'+(r.version||'?')); break; }
      } catch(e) {}
      await new Promise(function(r) { setTimeout(r, 1200); });
    }
    setTimeout(saverSync, 500);
  } catch(e) { if(e.message!=='The device has been lost.') tm('Connection failed', 'ng'); ss(false); }
});
window.addEventListener('beforeunload', async function() {
  if(dr) { try { await dr.cancel(); } catch(e){} } if(dw) { try { await dw.close(); } catch(e){} } if(dp) { try { await dp.close(); } catch(e){} }
});

// Read config
document.getElementById('btnRead').addEventListener('click', async function() {
  if(!dw) { tm('Connect first', 'ng'); return; }
  try { var r = await sc({cmd:'get_config'}, 15000); if(r.config) { cd=r.config; cs=0; sp=0; sb=-1; ensureV2(); ra(); tm('Config loaded'); } } catch(e) { console.error('Read failed:', e); tm('Read failed', 'ng'); }
});

function ensureV2() {
  if (!cd) return;
  if (!cd.version) cd.version = 2;
  if (!cd.pages) cd.pages = [];
  if (!cd.automations) cd.automations = [];
  if (sp >= cd.pages.length) sp = 0;
  cd.pages.forEach(function(p) { if (!p.buttons) p.buttons = []; });
}

// Write config
document.getElementById('btnWrite').addEventListener('click', async function() {
  if(!dw) { tm('Connect first', 'ng'); return; }
  if(__busy) return; __busy = true; var btn = this; btn.disabled = true;
  syncWifi(); syncSaver(); syncWidgets(); syncTheme(); syncAutomations();
  ensureV2();
  try { var r = await sc({cmd:'set_config',config:cd}, 15000); if(r && r.error) { var s = r.got||''; tm('CYD: '+r.error+' "'+s.substring(0,60)+'"', 'ng'); return; } tm('Written to CYD'); } catch(e) { console.error('Write failed:', e); tm('Write failed: '+(e.message||e), 'ng'); } finally { __busy = false; btn.disabled = false; }
});

// Export
document.getElementById('btnExport').addEventListener('click', function() {
  var b = new Blob([JSON.stringify(cd,null,2)], {type:'application/json'});
  var u = URL.createObjectURL(b); var a = document.createElement('a'); a.href=u; a.download='sudodeck-config.json'; a.click(); URL.revokeObjectURL(u); tm('Exported');
});

// Reset
document.getElementById('btnReset').addEventListener('click', async function() {
  if(!confirm('Factory reset?')) return;
  if(dw) { try { await sc({cmd:'factory_reset'}); var r = await sc({cmd:'get_config'}); if(r.config) cd=r.config; } catch(e) { tm('Reset failed','ng'); return; } }
  else { cd=dc(); }
  cs=0; sp=0; sb=-1; ra(); tm('Reset done');
});

// Import
document.getElementById('fileInput').addEventListener('change', function(e) {
  var f = e.target.files[0]; if(!f) return;
  var r = new FileReader();
  r.onload = function(ev) {
    try { var d = JSON.parse(ev.target.result); cd=d; cs=0; sp=0; sb=-1; ensureV2(); ra(); tm('Imported'); } catch(err) { tm('Invalid file','ng'); }
  };
  r.readAsText(f); e.target.value = '';
});

function modeToInt(m) {
  return {matrix:0,f1:4,custom:6}[m]||0;
}
function modeToStr(n) {
  return {0:'matrix',4:'f1',6:'custom'}[n]||'matrix';
}

function ul() {
  if(!cd) return;
  document.getElementById('lc').value=cd.grid?cd.grid.cols:4;
  document.getElementById('lr').value=cd.grid?cd.grid.rows:3;
  var pgs = cd.pages || [];
  document.getElementById('lp').value=pgs.length;
  if(!cd.wifi) cd.wifi={ssid:'',password:''};
  document.getElementById('wifiSsid').value=cd.wifi.ssid||'';
  document.getElementById('wifiPass').value=cd.wifi.password||'';
  if(!cd.saver) cd.saver={timeout:30,mode:0};
  if(cd.saver.sleep===undefined) cd.saver.sleep=60;
  document.getElementById('saverTimeout').value=cd.saver.timeout||30;
  document.getElementById('saverSleep').value=cd.saver.sleep||60;
  document.getElementById('saverMode').value=modeToStr(cd.saver.mode||0);
}
function syncWifi() {
  if(!cd) return;
  var s = document.getElementById('wifiSsid').value.trim();
  if (!s) { delete cd.wifi; return; }
  if(!cd.wifi) cd.wifi={};
  cd.wifi.ssid = s;
  cd.wifi.password = document.getElementById('wifiPass').value;
}
function syncSaver() {
  if(!cd) return;
  if(!cd.saver) cd.saver={timeout:30,sleep:60,mode:0};
  cd.saver.timeout=parseInt(document.getElementById('saverTimeout').value)||30;
  cd.saver.sleep=parseInt(document.getElementById('saverSleep').value)||0;
  cd.saver.mode=modeToInt(document.getElementById('saverMode').value);
}
function syncWidgets() {
  if(!cd) return;
  if(!cd.widgets) cd.widgets=[];
  var el = document.getElementById('widgetList');
  el.innerHTML = '';
  var h = document.getElementById('widgetHint');
  h.textContent = cd.widgets.length + ' configured';
  cd.widgets.forEach(function(w,i) {
    var c = document.createElement('div');
    c.className = 'widget-card';
    c.innerHTML =
      '<div class="wc-row">'+
        '<span class="wc-label">'+esc(w.label||'?')+'</span>'+
        '<span class="wc-url">'+esc((w.url||'').substring(0,50))+'</span>'+
        '<span class="wc-interval">'+esc(w.interval||60)+'s</span>'+
        '<div class="wc-actions">'+
          '<button class="btn wc-edit-btn" data-i="'+i+'">Edit</button>'+
          '<button class="btn dan wc-rm-btn" data-i="'+i+'">Del</button>'+
        '</div>'+
      '</div>'+
      '<div class="wc-edit" id="wcEdit'+i+'">'+
        '<div class="wc-ef"><label>Label</label><input class="wc-fi" data-i="'+i+'" data-f="label" value="'+esc(w.label||'')+'"></div>'+
        '<div class="wc-ef"><label>URL</label><textarea class="wc-fi" data-i="'+i+'" data-f="url" rows="2">'+esc(w.url||'')+'</textarea></div>'+
        '<div class="wc-ef"><label>Path</label><input class="wc-fi" data-i="'+i+'" data-f="path" value="'+esc(w.path||'')+'" placeholder="e.g. bitcoin.usd"></div>'+
        '<div class="wc-ef"><label>Format</label><input class="wc-fi" data-i="'+i+'" data-f="format" value="'+esc(w.format||'')+'" placeholder="${value}"></div>'+
        '<div class="wc-ef"><label>Interval</label><input class="wc-fi" data-i="'+i+'" data-f="interval" value="'+(w.interval||60)+'" style="flex:0;width:60px"> <span style="font-size:0.65rem;color:var(--dim)">sec</span></div>'+
        '<div style="display:flex;gap:4px;margin-top:4px">'+
          '<button class="btn pri wc-save-btn" data-i="'+i+'">Save</button>'+
          '<button class="btn wc-test-btn" data-i="'+i+'">Test</button>'+
        '</div>'+
        '<div class="wc-test" id="wcTest'+i+'"></div>'+
      '</div>';
    el.appendChild(c);
  });
}
function esc(s) { return String(s).replace(/&/g,'&amp;').replace(/</g,'&lt;').replace(/>/g,'&gt;').replace(/"/g,'&quot;'); }

// Apply Layout
document.getElementById('bal').addEventListener('click', function() {
  var c=Math.max(1,Math.min(6,parseInt(document.getElementById('lc').value)||4));
  var r=Math.max(1,Math.min(5,parseInt(document.getElementById('lr').value)||3));
  var pc=Math.max(1,Math.min(18,parseInt(document.getElementById('lp').value)||1));
  if(!cd.grid) cd.grid={};
  cd.grid.cols=c; cd.grid.rows=r; var tb=c*r;
  while(cd.pages.length<pc) { var bt=[]; for(var i=0;i<tb;i++) bt.push({label:'',color:'#16213E',action:{type:'key',value:''}}); cd.pages.push({name:'Page '+(cd.pages.length+1),buttons:bt}); }
  while(cd.pages.length>pc) cd.pages.pop();
  for(var i=0;i<cd.pages.length;i++) { while(cd.pages[i].buttons.length<tb) cd.pages[i].buttons.push({label:'',color:'#16213E',action:{type:'key',value:''}}); }
  syncWifi(); syncSaver();
  sp=0; sb=-1; ra(); tm('Layout updated');
});

function ra() { renderPageBar(); rg(); ul(); syncWidgets(); syncThemeUI(); syncAutoUI(); }

function renderPageBar() {
  var b = document.getElementById('pageBar'); b.innerHTML = '';
  if (!cd || !cd.pages) return;
  cd.pages.forEach(function(p, i) {
    var t = document.createElement('span');
    t.className = 'p-tab' + (i === sp ? ' on' : '');
    t.textContent = p.name || ('Pg' + (i + 1));
    t.addEventListener('click', function(idx) { return function() { sp = idx; sb = -1; ra(); }; }(i));
    t.addEventListener('dblclick', function(idx) { return function() { var n = prompt('Page name:', cd.pages[idx].name); if (n !== null) { cd.pages[idx].name = n; ra(); } }; }(i));
    b.appendChild(t);
  });
}

function rg() {
  var g = document.getElementById('gp');
  if (!cd || !cd.grid) return;
  var cols = cd.grid.cols, rows = cd.grid.rows;
  g.style.gridTemplateColumns = 'repeat(' + cols + ',1fr)';
  var p = curPage();
  if (!p) { g.innerHTML = ''; return; }
  g.innerHTML = '';
  for (var i = 0; i < cols * rows; i++) {
    var b = document.createElement('div');
    b.className = 'gb';
    if (i >= p.buttons.length || !p.buttons[i] || !p.buttons[i].label) b.classList.add('em');
    if (i === sb) b.classList.add('sel');
    var d = p.buttons[i] || { label: '', color: '#16213E' };
    b.style.backgroundColor = d.color || '#16213E';
    b.innerHTML = '<span class="ix">' + (i + 1) + '</span>' + (d.label || '...');
    b.addEventListener('click', function(idx) { return function() { sb = idx; rg(); se(idx); }; }(i));
    g.appendChild(b);
  }
}

function se(idx) {
  var p = document.getElementById('ep');
  var pg = curPage();
  if (!pg || idx >= pg.buttons.length) { p.style.display = 'none'; return; }
  p.style.display = 'flex';
  var b = pg.buttons[idx];
  document.getElementById('el').value = b.label || '';
  document.getElementById('ec').value = b.color || '#16213E';
  document.getElementById('eat').value = b.action ? b.action.type : 'key';
  uaf();
}

function uaf() {
  var t = document.getElementById('eat').value;
  var c = document.getElementById('af');
  var b = curBtn();
  var a = b ? b.action : { type: 'key', value: '' };
  var h = '';
  if (t === 'key') {
    h = '<div class="er"><label>Key</label><input type="text" id="ekv" value="' + es(a.value || '') + '" placeholder="ENTER, F1, a"></div>';
  } else if (t === 'combo') {
    h = '<div class="er"><label>Mod</label><select id="ecm">' +
      ['CTRL', 'ALT', 'SHIFT', 'GUI', 'CTRL_SHIFT', 'CTRL_ALT', 'CTRL_GUI', 'ALT_SHIFT', 'ALT_GUI', 'SHIFT_GUI', 'CTRL_ALT_SHIFT', 'CTRL_ALT_GUI', 'CTRL_SHIFT_GUI', 'ALT_SHIFT_GUI', 'CTRL_ALT_SHIFT_GUI'].map(function(m) { return '<option value="' + m + '"' + (a.mod === m ? ' selected' : '') + '>' + m.replace(/_/g, '+') + '</option>'; }).join('') +
      '</select><label>Key</label><input type="text" id="eck" value="' + es(a.key || '') + '" placeholder="c, s, v"></div>';
  } else if (t === 'text') {
    h = '<div class="er"><label>Text</label><input type="text" id="etv" value="' + es(a.value || '') + '" placeholder="Text to type"></div>';
  } else if (t === 'delay') {
    h = '<div class="er"><label>Delay</label><input type="number" id="edv" value="' + (a.value || 100) + '" min="1" max="10000"> <span style="font-size:0.7rem;color:var(--dim)">ms</span></div>';
  } else if (t === 'app') {
    h = '<div class="er"><label>OS</label><select id="aos">' +
      ['windows', 'macos', 'linux'].map(function(o) { return '<option value="' + o + '"' + (a.os === o ? ' selected' : '') + '>' + o.charAt(0).toUpperCase() + o.slice(1) + '</option>'; }).join('') +
      '</select></div>' +
      '<div class="er"><label>App name</label><input type="text" id="anv" value="' + es(a.name || '') + '" placeholder="Calculator"> <span style="font-size:0.65rem;color:var(--dim)">use the display name, like Calculator or Notepad</span></div>';
  } else if (t === 'macro') {
    h = '<div class="ab"><div style="display:flex;justify-content:space-between;align-items:center;margin-bottom:6px;"><span style="color:var(--acc);font-size:0.7rem;">STEPS</span><button class="btn" id="bas">+ Add</button></div><div id="mss"></div></div>';
    setTimeout(function() {
      rms(a.steps || []);
      document.getElementById('bas').addEventListener('click', function() {
        var s = gaf().steps || []; s.push({ type: 'key', value: '' });
        var btn = curBtn(); if (btn) btn.action = { type: 'macro', steps: s };
        uaf();
      });
    }, 50);
  } else if (t === 'automation_toggle') {
    syncAutomations();
    var opts = (cd.automations || []).map(function(au, j) {
      return '<option value="' + j + '"' + (a.index === j ? ' selected' : '') + '>' + esc(au.name || ('Auto ' + (j + 1))) + '</option>';
    }).join('');
    h = '<div class="er"><label>Automation</label><select id="eai">' + opts + '</select></div>';
  }
  c.innerHTML = h;
}

function rms(steps) {
  var c = document.getElementById('mss'); if (!c) return;
  c.innerHTML = '';
  steps = steps || [];
  var mods = ['CTRL', 'ALT', 'SHIFT', 'GUI', 'CTRL_SHIFT', 'CTRL_ALT', 'CTRL_GUI', 'ALT_SHIFT', 'ALT_GUI', 'SHIFT_GUI', 'CTRL_ALT_SHIFT', 'CTRL_ALT_GUI', 'CTRL_SHIFT_GUI', 'ALT_SHIFT_GUI', 'CTRL_ALT_SHIFT_GUI'];
  steps.forEach(function(s, i) {
    var d = document.createElement('div'); d.className = 'ms';
    var inner = '<span style="color:var(--dim);font-size:0.6rem;min-width:18px;font-family:var(--mono)">#' + (i + 1) + '</span>' +
      '<select class="mt" data-idx="' + i + '">' + ['key', 'combo', 'text', 'delay'].map(function(t) { return '<option value="' + t + '"' + (s.type === t ? ' selected' : '') + '>' + t.charAt(0).toUpperCase() + t.slice(1) + '</option>'; }).join('') + '</select>';
    if (s.type === 'combo') {
      inner += '<select class="mm" data-idx="' + i + '" style="flex:0 1 auto;max-width:100px;">' +
        mods.map(function(m) { return '<option value="' + m + '"' + (s.mod === m ? ' selected' : '') + '>' + m.replace(/_/g, '+') + '</option>'; }).join('') +
        '</select>' +
        '<input type="text" class="mv" value="' + es(s.key || '') + '" data-idx="' + i + '" placeholder="key (c, v...)" style="flex:1">';
    } else if (s.type === 'delay') {
      inner += '<input type="number" class="mv" value="' + (s.value || 100) + '" data-idx="' + i + '" min="1" max="10000" style="width:80px;">' +
        '<span style="color:var(--dim);font-size:0.65rem;">ms</span>';
    } else {
      inner += '<input type="text" class="mv" value="' + es(s.value || '') + '" data-idx="' + i + '" placeholder="value" style="flex:1">';
    }
    inner += '<button class="btn dan md" data-idx="' + i + '" style="padding:3px 8px;font-size:0.65rem;">X</button>';
    d.innerHTML = inner;
    c.appendChild(d);
  });
  c.querySelectorAll('.mt').forEach(function(s) {
    s.addEventListener('change', function(e) {
      var idx = parseInt(e.target.dataset.idx);
      var btn = curBtn(); if (btn && btn.action && btn.action.steps) { btn.action.steps[idx].type = e.target.value; rms(btn.action.steps); }
    });
  });
  c.querySelectorAll('.mv').forEach(function(i) {
    i.addEventListener('input', function(e) {
      var idx = parseInt(e.target.dataset.idx);
      var btn = curBtn();
      if (btn && btn.action && btn.action.steps) {
        var s2 = btn.action.steps[idx];
        if (s2.type === 'key' || s2.type === 'text') s2.value = e.target.value;
        else if (s2.type === 'combo') s2.key = e.target.value;
        else if (s2.type === 'delay') s2.value = parseInt(e.target.value) || 100;
      }
    });
  });
  c.querySelectorAll('.mm').forEach(function(s) {
    s.addEventListener('change', function(e) {
      var idx = parseInt(e.target.dataset.idx);
      var btn = curBtn(); if (btn && btn.action && btn.action.steps) { btn.action.steps[idx].mod = e.target.value; }
    });
  });
  c.querySelectorAll('.md').forEach(function(b) {
    b.addEventListener('click', function(e) {
      var idx = parseInt(e.target.dataset.idx);
      var btn = curBtn(); if (btn && btn.action && btn.action.steps) { btn.action.steps.splice(idx, 1); rms(btn.action.steps); }
    });
  });
}

function gaf() {
  var t = document.getElementById('eat').value;
  if (t === 'key') return { type: 'key', value: document.getElementById('ekv')?.value || '' };
  if (t === 'combo') return { type: 'combo', mod: document.getElementById('ecm')?.value || 'CTRL', key: document.getElementById('eck')?.value || '' };
  if (t === 'text') return { type: 'text', value: document.getElementById('etv')?.value || '' };
  if (t === 'delay') return { type: 'delay', value: parseInt(document.getElementById('edv')?.value) || 100 };
  if (t === 'app') return { type: 'app', os: document.getElementById('aos')?.value || 'windows', name: document.getElementById('anv')?.value || '' };
  if (t === 'macro') { var btn = curBtn(); return { type: 'macro', steps: btn?.action?.steps || [] }; }
  if (t === 'automation_toggle') return { type: 'automation_toggle', index: parseInt(document.getElementById('eai')?.value) || 0 };
  return { type: 'key', value: '' };
}
document.getElementById('eat').addEventListener('change', function() {
  var b = curBtn();
  if (b) {
    var nt = document.getElementById('eat').value;
    var na = { type: nt };
    if (nt === 'key' || nt === 'text') na.value = '';
    if (nt === 'combo') { na.mod = 'CTRL'; na.key = ''; }
    if (nt === 'delay') na.value = 100;
    if (nt === 'app') { na.os = 'windows'; na.name = ''; }
    if (nt === 'macro') na.steps = [];
    if (nt === 'automation_toggle') na.index = 0;
    b.action = na;
  }
  uaf();
});
document.getElementById('bae').addEventListener('click', function() {
  var b = curBtn(); if (!b) return;
  b.label = document.getElementById('el').value;
  b.color = document.getElementById('ec').value;
  b.action = gaf();
  document.getElementById('ep').style.display = 'none';
  ra(); tm('Button updated');
});
document.getElementById('bce').addEventListener('click', function() {
  document.getElementById('ep').style.display = 'none';
  sb = -1; rg();
});
function es(s) { return (s || '').replace(/&/g, '&amp;').replace(/</g, '&lt;').replace(/>/g, '&gt;').replace(/"/g, '&#34;'); }

var nav = document.querySelector('nav');
var navTicking = false;
window.addEventListener('scroll', function() {
  if (!navTicking) {
    requestAnimationFrame(function() {
      nav.classList.toggle('scrolled', window.scrollY > 60);
      navTicking = false;
    });
    navTicking = true;
  }
});

var hero = document.querySelector('.hero');
if (hero) {
  var heroGlow = document.createElement('div');
  heroGlow.style.cssText = 'position:absolute;top:0;left:0;width:100%;height:100%;pointer-events:none;z-index:0;transition:opacity 0.3s;opacity:0;';
  hero.appendChild(heroGlow);
  var hTicking = false;
  document.addEventListener('mousemove', function(e) {
    if (hero.getBoundingClientRect().bottom < 0) { heroGlow.style.opacity = '0'; return; }
    if (!hTicking) {
      requestAnimationFrame(function() {
        var r = hero.getBoundingClientRect();
        var x = ((e.clientX - r.left) / r.width) * 100;
        var y = ((e.clientY - r.top) / r.height) * 100;
        heroGlow.style.background = 'radial-gradient(circle at ' + x + '% ' + y + '%, rgba(0,255,136,0.06) 0%, transparent 60%)';
        heroGlow.style.opacity = '1';
        hTicking = false;
      });
    }
  });
}

// ── Screensaver ──

document.getElementById('saverToggle').addEventListener('click', function() {
  var b = document.getElementById('saverBody');
  b.classList.toggle('open');
});
async function saverSync() {
  if (!dw) return;
  try { var r = await sc({ cmd: 'get_saver' });
    document.getElementById('saverTimeout').value = r.timeout || 30;
    document.getElementById('saverSleep').value = r.sleep != null ? r.sleep : 60;
    document.getElementById('saverHint').textContent = r.timeout + 's / sleep ' + (r.sleep || 'off');
    if (r.mode) document.getElementById('saverMode').value = r.mode;
    if (cd && cd.saver) { cd.saver.timeout = r.timeout || 30; cd.saver.sleep = r.sleep || 60; cd.saver.mode = modeToInt(r.mode || 'matrix'); }
  } catch (e) {}
}
document.getElementById('btnSaverSet').addEventListener('click', async function() {
  if (!dw) { tm('Connect first', 'ng'); return; }
  var t = parseInt(document.getElementById('saverTimeout').value) || 30;
  var s = parseInt(document.getElementById('saverSleep').value) || 0;
  if (t < 5) t = 5; if (t > 600) t = 600;
  if (s < 0) s = 0; if (s > 600) s = 600;
  try { await sc({ cmd: 'set_saver', timeout: t, sleep: s }); cd.saver.timeout = t; cd.saver.sleep = s; tm('Timeout ' + t + 's, sleep ' + (s || 'off')); saverSync(); } catch (e) { tm('Failed', 'ng'); }
});
document.getElementById('btnSaverModeSet').addEventListener('click', async function() {
  if (!dw) { tm('Connect first', 'ng'); return; }
  var m = document.getElementById('saverMode').value;
  try { var r = await sc({ cmd: 'set_saver_mode', mode: m }); if (r && r.error) { tm('Mode error: ' + r.error, 'ng'); return; } cd.saver.mode = modeToInt(m); tm('Saver mode: ' + m); saverSync(); } catch (e) { tm('Mode command failed', 'ng'); }
});

// ── Theme ──

var themePresets = {
  default:    { bg: "#000000", hdr: "#084228", acc: "#00FF88", txt: "#FFFFFF", dim: "#637282", btn: "#084228" },
  midnight:   { bg: "#000000", hdr: "#000028", acc: "#A17CFF", txt: "#FFFFFF", dim: "#637282", btn: "#100820" },
  forest:     { bg: "#000000", hdr: "#022000", acc: "#00FF00", txt: "#FFFFFF", dim: "#637282", btn: "#0A2000" },
  amber:      { bg: "#000000", hdr: "#200000", acc: "#FFC000", txt: "#FFFFFF", dim: "#637282", btn: "#200000" },
  cyberpunk:  { bg: "#000000", hdr: "#200820", acc: "#FF0088", txt: "#FFFFFF", dim: "#637282", btn: "#180820" },
  monochrome: { bg: "#101210", hdr: "#212424", acc: "#BDBFBF", txt: "#FFFFFF", dim: "#637282", btn: "#212424" },
  ocean:      { bg: "#000000", hdr: "#022010", acc: "#00EFEF", txt: "#FFFFFF", dim: "#637282", btn: "#082000" },
  blood:      { bg: "#000000", hdr: "#300000", acc: "#FF0000", txt: "#FFFFFF", dim: "#637282", btn: "#180000" },
  nord:       { bg: "#000000", hdr: "#182838", acc: "#5A7B8B", txt: "#FFFFFF", dim: "#637282", btn: "#212428" },
  dracula:    { bg: "#000000", hdr: "#200828", acc: "#FF00FF", txt: "#FFFFFF", dim: "#637282", btn: "#200828" },
};

function syncThemeUI() {
  if (!cd) return;
  if (!cd.theme) cd.theme = { name: "default", button_style: "flat" };
  document.getElementById('themeName').value = cd.theme.name || 'default';
  document.getElementById('themeStyle').value = cd.theme.button_style || 'flat';
  var p = themePresets[cd.theme.name] || themePresets.default;
  var tp = document.getElementById('themePreview');
  tp.style.cssText = 'display:flex;gap:6px;align-items:center;padding:8px;border-radius:6px;background:' + p.bg + ';';
  tp.innerHTML =
    '<span style="display:inline-block;width:16px;height:16px;border-radius:3px;background:' + p.hdr + '"></span>' +
    '<span style="display:inline-block;width:16px;height:16px;border-radius:3px;background:' + p.acc + '"></span>' +
    '<span style="display:inline-block;width:16px;height:16px;border-radius:3px;background:' + p.txt + ';border:1px solid #333"></span>' +
    '<span style="display:inline-block;width:16px;height:16px;border-radius:3px;background:' + p.dim + '"></span>' +
    '<span style="display:inline-block;width:16px;height:16px;border-radius:3px;background:' + p.btn + '"></span>' +
    '<span style="color:' + p.txt + ';font-size:0.7rem;margin-left:4px;">' + cd.theme.name + '</span>';
  var styles = ['flat', 'outlined', 'neon', 'soft', 'shadow'];
  document.getElementById('themeStylePreview').innerHTML = styles.map(function(s) {
    var sel = s === (cd.theme.button_style || 'flat') ? ' style="border:2px solid var(--acc);"' : '';
    return '<span class="ts-pill" data-style="' + s + '"' + sel + '>' + s + '</span>';
  }).join('');
}

function syncTheme() {
  if (!cd) return;
  cd.theme.name = document.getElementById('themeName').value;
  cd.theme.button_style = document.getElementById('themeStyle').value;
}

document.getElementById('themeName').addEventListener('change', function() { syncTheme(); syncThemeUI(); });
document.getElementById('themeStyle').addEventListener('change', function() { syncTheme(); syncThemeUI(); });
document.getElementById('btnThemeSet').addEventListener('click', async function() {
  if (!dw) { tm('Connect first', 'ng'); return; }
  syncTheme();
  try { await sc({ cmd: 'set_theme', theme: cd.theme }); tm('Theme: ' + cd.theme.name + ', ' + cd.theme.button_style); } catch (e) { tm('Theme command failed', 'ng'); }
});
document.getElementById('themeToggle').addEventListener('click', function() {
  document.getElementById('themeBody').classList.toggle('open');
});

// ── Widgets ──

document.getElementById('widgetToggle').addEventListener('click', function() {
  document.getElementById('widgetBody').classList.toggle('open');
});
document.getElementById('btnWidgetAdd').addEventListener('click', function() {
  if (!cd) return;
  if (!cd.widgets) cd.widgets = [];
  cd.widgets.push({ label: '', url: '', path: '', format: '{value}', interval: 60 });
  syncWidgets();
  var list = document.getElementById('widgetList');
  var last = list.lastElementChild;
  if (last) {
    var ed = last.querySelector('.wc-edit');
    if (ed) ed.classList.add('open');
  }
});
document.getElementById('widgetList').addEventListener('click', function(e) {
  var btn = e.target.closest('button');
  if (!btn) return;
  var i = parseInt(btn.getAttribute('data-i'));
  if (isNaN(i) || !cd || !cd.widgets || i >= cd.widgets.length) return;
  if (btn.classList.contains('wc-edit-btn')) {
    var ed = document.getElementById('wcEdit' + i);
    if (ed) ed.classList.toggle('open');
  } else if (btn.classList.contains('wc-rm-btn')) {
    cd.widgets.splice(i, 1);
    syncWidgets();
  } else if (btn.classList.contains('wc-save-btn')) {
    var ed = document.getElementById('wcEdit' + i);
    if (!ed) return;
    var w = cd.widgets[i];
    ed.querySelectorAll('.wc-fi').forEach(function(inp) {
      var f = inp.getAttribute('data-f');
      if (f === 'interval') w[f] = parseInt(inp.value) || 60;
      else w[f] = inp.value;
    });
    syncWidgets();
  } else if (btn.classList.contains('wc-test-btn')) {
    var w = cd.widgets[i];
    var te = document.getElementById('wcTest' + i);
    if (!te || !w.url) { if (te) { te.textContent = 'no url'; te.className = 'wc-test ng'; } return; }
    te.textContent = 'testing...'; te.className = 'wc-test';
    fetch(w.url, { signal: AbortSignal.timeout(10000) })
      .then(function(r) { if (!r.ok) throw Error('HTTP ' + r.status); return r.text(); })
      .then(function(body) {
        try {
          var pd = JSON.parse(body);
          var val = pd;
          var segs = (w.path || '').split('.');
          for (var k = 0; k < segs.length; k++) {
            var s = segs[k];
            var br = s.indexOf('[');
            if (br >= 0) { var idx = parseInt(s.substring(br + 1)); s = s.substring(0, br); if (s) val = val[s]; val = val[idx]; } else if (s) val = val[s];
            if (val === undefined || val === null) { val = undefined; break; }
          }
          if (val === undefined) te.textContent = 'path not found';
          else {
            var fmt = w.format || '{value}';
            te.textContent = fmt.replace('{value}', String(val));
          }
          te.className = 'wc-test ok';
        } catch (e) { te.textContent = 'parse error: ' + e.message; te.className = 'wc-test ng'; }
      })
      .catch(function(e) { te.textContent = 'fetch error: ' + (e.message || e); te.className = 'wc-test ng'; });
  }
});

// ── Automations ──

function syncAutomations() {
  if (!cd) return;
  if (!cd.automations) cd.automations = [];
}
function syncAutoUI() {
  if (!cd) return;
  syncAutomations();
  var h = document.getElementById('autoHint');
  h.textContent = cd.automations.length;
  var el = document.getElementById('autoList');
  el.innerHTML = '';
  cd.automations.forEach(function(a, i) {
    var c = document.createElement('div');
    c.className = 'widget-card';
    var typeLabel = a.type === 'timer' ? ('Every ' + (a.interval || 60) + 's') : (a.time || '?');
    c.innerHTML =
      '<div class="wc-row">' +
        '<span style="font-size:0.65rem;color:var(--acc);min-width:44px">' + a.type + '</span>' +
        '<span class="wc-label" style="flex:1">' + esc(a.name || '') + '</span>' +
        '<span style="font-size:0.65rem;color:var(--dim);margin-right:8px">' + esc(typeLabel) + '</span>' +
        '<div class="wc-actions">' +
          '<button class="btn auto-on-btn" data-i="' + i + '" style="' + (a.enabled !== false ? 'background:var(--acc);color:#fff' : '') + '">' + (a.enabled !== false ? 'ON' : 'OFF') + '</button>' +
          '<button class="btn auto-edit-btn" data-i="' + i + '">Edit</button>' +
          '<button class="btn dan auto-rm-btn" data-i="' + i + '">Del</button>' +
        '</div>' +
      '</div>' +
      '<div class="wc-edit" id="autoEdit' + i + '">' +
        '<div class="wc-ef"><label>Name</label><input class="auto-fi" data-i="' + i + '" data-f="name" value="' + esc(a.name || '') + '"></div>' +
        '<div class="wc-ef"><label>Type</label><select class="auto-fi" data-i="' + i + '" data-f="type">' +
          '<option value="timer"' + (a.type === 'timer' ? ' selected' : '') + '>Timer</option>' +
          '<option value="schedule"' + (a.type === 'schedule' ? ' selected' : '') + '>Schedule</option>' +
        '</select></div>' +
        '<div class="wc-ef" id="autoTimerFields' + i + '"' + (a.type === 'schedule' ? ' style="display:none"' : '') + '><label>Interval</label><input class="auto-fi" data-i="' + i + '" data-f="interval" value="' + (a.interval || 60) + '" style="flex:none;width:60px"> <span style="font-size:0.65rem;color:var(--dim)">sec</span></div>' +
        '<div class="wc-ef" id="autoSchedFields' + i + '"' + (a.type !== 'schedule' ? ' style="display:none"' : '') + '>' +
          '<label>Time</label><input class="auto-fi" data-i="' + i + '" data-f="time" value="' + esc(a.time || '09:00') + '" placeholder="HH:MM" style="flex:none;width:60px">' +
          '<label style="min-width:auto;margin-left:8px">Days</label><input class="auto-fi" data-i="' + i + '" data-f="days" value="' + esc(a.days || '1,2,3,4,5') + '" placeholder="1-7, comma" style="flex:1">' +
        '</div>' +
        '<div class="wc-ef"><label>Action</label><select class="auto-action-type" data-i="' + i + '">' +
          ['key', 'combo', 'text', 'delay', 'app'].map(function(t) { return '<option value="' + t + '"' + ((a.action || {}).type === t ? ' selected' : '') + '>' + t.charAt(0).toUpperCase() + t.slice(1) + '</option>'; }).join('') +
        '</select></div>' +
        '<div id="autoActionFields' + i + '"></div>' +
        '<div style="display:flex;gap:4px;margin-top:4px">' +
          '<button class="btn pri auto-save-btn" data-i="' + i + '">Save</button>' +
        '</div>' +
      '</div>';
    el.appendChild(c);
    renderAutoActionFields(i, a.action || { type: 'key', value: '' });
  });
}

function renderAutoActionFields(idx, action) {
  var el = document.getElementById('autoActionFields' + idx);
  if (!el) return;
  var t = (action || {}).type || 'key';
  var h = '';
  if (t === 'key') {
    h = '<div class="wc-ef"><label>Key</label><input class="auto-af" data-i="' + idx + '" data-f="value" value="' + es(action.value || '') + '" placeholder="ENTER, F1, a"></div>';
  } else if (t === 'combo') {
    h = '<div class="wc-ef"><label>Mod</label><select class="auto-af" data-i="' + idx + '" data-f="mod">' +
      ['CTRL', 'ALT', 'SHIFT', 'GUI', 'CTRL_SHIFT', 'CTRL_ALT'].map(function(m) { return '<option value="' + m + '"' + (action.mod === m ? ' selected' : '') + '>' + m.replace(/_/g, '+') + '</option>'; }).join('') +
      '</select><label>Key</label><input class="auto-af" data-i="' + idx + '" data-f="key" value="' + es(action.key || '') + '" placeholder="c, s" style="flex:1"></div>';
  } else if (t === 'text') {
    h = '<div class="wc-ef"><label>Text</label><input class="auto-af" data-i="' + idx + '" data-f="value" value="' + es(action.value || '') + '" placeholder="Text" style="flex:1"></div>';
  } else if (t === 'delay') {
    h = '<div class="wc-ef"><label>Delay</label><input class="auto-af" data-i="' + idx + '" data-f="value" value="' + (action.value || 100) + '" style="flex:none;width:60px"> <span style="font-size:0.65rem;color:var(--dim)">ms</span></div>';
  } else if (t === 'app') {
    h = '<div class="wc-ef"><label>OS</label><select class="auto-af" data-i="' + idx + '" data-f="os">' +
      ['windows', 'macos', 'linux'].map(function(o) { return '<option value="' + o + '"' + (action.os === o ? ' selected' : '') + '>' + o.charAt(0).toUpperCase() + o.slice(1) + '</option>'; }).join('') +
      '</select><label>Name</label><input class="auto-af" data-i="' + idx + '" data-f="name" value="' + es(action.name || '') + '" placeholder="Calculator" style="flex:1"></div>';
  }
  el.innerHTML = h;
}

document.getElementById('autoToggle').addEventListener('click', function() {
  document.getElementById('autoBody').classList.toggle('open');
});

document.getElementById('btnAutoAdd').addEventListener('click', function() {
  if (!cd) return;
  syncAutomations();
  cd.automations.push({ type: 'timer', name: '', interval: 60, time: '09:00', days: '1,2,3,4,5', enabled: true, action: { type: 'key', value: '' } });
  syncAutoUI();
  var list = document.getElementById('autoList');
  var last = list.lastElementChild;
  if (last) {
    var ed = last.querySelector('.wc-edit');
    if (ed) ed.classList.add('open');
  }
});

document.getElementById('autoList').addEventListener('click', function(e) {
  var btn = e.target.closest('button');
  if (!btn) return;
  var i = parseInt(btn.getAttribute('data-i'));
  if (isNaN(i) || !cd || !cd.automations || i >= cd.automations.length) return;
  if (btn.classList.contains('auto-on-btn')) {
    cd.automations[i].enabled = cd.automations[i].enabled !== false ? false : true;
    syncAutoUI();
  } else if (btn.classList.contains('auto-edit-btn')) {
    var ed = document.getElementById('autoEdit' + i);
    if (ed) ed.classList.toggle('open');
  } else if (btn.classList.contains('auto-rm-btn')) {
    cd.automations.splice(i, 1);
    syncAutoUI();
  } else if (btn.classList.contains('auto-save-btn')) {
    var ed = document.getElementById('autoEdit' + i);
    if (!ed) return;
    var a = cd.automations[i];
    ed.querySelectorAll('.auto-fi').forEach(function(inp) {
      var f = inp.getAttribute('data-f');
      if (f === 'interval') a[f] = parseInt(inp.value) || 60;
      else if (f === 'time') a[f] = inp.value || '09:00';
      else if (f === 'days') a[f] = inp.value || '';
      else a[f] = inp.value;
    });
    // Save action
    var actionTypeEl = ed.querySelector('.auto-action-type');
    var action = { type: actionTypeEl ? actionTypeEl.value : 'key' };
    ed.querySelectorAll('.auto-af').forEach(function(inp) {
      var f = inp.getAttribute('data-f');
      if (f === 'value' || f === 'key' || f === 'mod' || f === 'os' || f === 'name') {
        action[f] = f === 'value' ? (inp.value || '') : inp.value;
      }
    });
    a.action = action;
    syncAutoUI();
  }
});

// Handle automation type change
document.getElementById('autoList').addEventListener('change', function(e) {
  var el = e.target;
  if (el.classList.contains('auto-fi') && el.getAttribute('data-f') === 'type') {
    var i = parseInt(el.getAttribute('data-i'));
    if (!isNaN(i)) {
      var timerFields = document.getElementById('autoTimerFields' + i);
      var schedFields = document.getElementById('autoSchedFields' + i);
      if (timerFields) timerFields.style.display = el.value === 'schedule' ? 'none' : '';
      if (schedFields) schedFields.style.display = el.value !== 'schedule' ? 'none' : '';
    }
  }
  if (el.classList.contains('auto-action-type')) {
    var i = parseInt(el.getAttribute('data-i'));
    if (!isNaN(i)) {
      var a = cd.automations[i];
      var action = { type: el.value };
      if (el.value === 'key' || el.value === 'text') action.value = '';
      else if (el.value === 'combo') { action.mod = 'CTRL'; action.key = ''; }
      else if (el.value === 'delay') action.value = 100;
      else if (el.value === 'app') { action.os = 'windows'; action.name = ''; }
      a.action = action;
      renderAutoActionFields(i, action);
    }
  }
});

ic(); ra();

/* ── Daemon coordination ── */
function tryReleaseDaemon() {
  fetch('http://127.0.0.1:8092/release', { method: 'POST' }).catch(function(){});
}
/* ── Firmware (esptool-js via dynamic import) ── */
var fwLoader = null, fwFileData = null, fwTransport = null, fwFlashAddr = 0, fwManifest = null;

function fwLog(msg) {
  var el = document.getElementById('fwLog');
  el.textContent += msg + '\n';
  el.scrollTop = el.scrollHeight;
}

function fwBar(pct) {
  document.getElementById('fwBar').style.width = Math.min(pct, 100) + '%';
}

function fwSetStatus(s) {
  var el = document.getElementById('fwStatus');
  el.textContent = s;
  el.style.color = s === 'Connected' ? 'var(--acc)' : 'var(--dim)';
}

function fwBaseUrl() {
  var host = window.location.hostname;
  if (host === '127.0.0.1' || host === 'localhost') {
    return 'https://sudobreakstuff.github.io/sudodeck/assets/firmware';
  }
  var parts = window.location.pathname.split('/').filter(Boolean);
  return '/' + (parts.length > 1 ? parts[0] : '') + '/assets/firmware';
}
async function fwLoadManifest() {
  try {
    var r = await fetch(fwBaseUrl() + '/manifest.json');
    if (!r.ok) { fwLog('HTTP ' + r.status); return; }
    fwManifest = await r.json();
    var sel = document.getElementById('fwVersion');
    var groups = {};
    fwManifest.versions.forEach(function(v) {
      var variant = v.variant || 'ble';
      if (!groups[variant]) groups[variant] = document.createElement('optgroup');
      var opt = document.createElement('option');
      opt.value = v.file;
      opt.textContent = v.label;
      opt.dataset.address = v.address;
      groups[variant].appendChild(opt);
    });
    var order = ['ble', 'wired'];
    var labels = { ble: 'Standard (BLE)', wired: 'Wired (Serial)' };
    order.forEach(function(k) {
      if (!groups[k]) return;
      groups[k].label = labels[k] || k;
      sel.appendChild(groups[k]);
    });
    fwLog('Firmware versions loaded.');
    fwApplyVariant();
  } catch (e) {
    fwLog('Could not load firmware list.');
  }
}
fwLoadManifest();

function fwApplyVariant() {
  var v = document.querySelector('input[name="fwVariant"]:checked');
  if (!v) return;
  var variant = v.value;
  var sel = document.getElementById('fwVersion');
  var wiredGroup = null;
  var hasWired = false;
  [].forEach.call(sel.querySelectorAll('optgroup'), function(g) {
    var isWired = g.label.indexOf('Wired') >= 0;
    if (isWired) wiredGroup = g;
    var show = (variant === 'all' || (variant === 'wired') === isWired);
    g.style.display = show ? '' : 'none';
    if (show && isWired && g.options.length > 0) hasWired = true;
  });
  // if current selection is hidden, reset
  if (sel.selectedIndex >= 0) {
    var opt = sel.options[sel.selectedIndex];
    if (opt.parentElement && opt.parentElement.style.display === 'none') {
      sel.value = '';
      fwVersionChanged();
    }
  }
  var note = document.getElementById('fwVariantNote');
  if (variant === 'wired') {
    if (!hasWired) {
      note.innerHTML = 'No wired firmware in manifest yet. <a href="../guide/#section-wired" style="color:var(--acc)">Build it from source</a> or use <strong>Custom .bin</strong> below.';
    } else {
      note.textContent = 'Requires the companion daemon for keystroke injection.';
    }
  } else {
    note.textContent = 'No extra software needed — works as a Bluetooth keyboard out of the box.';
  }
  daemonShow(variant === 'wired');
}

var daemonInfo = (function() {
  var ua = navigator.userAgent;
  var os = 'unknown', arch = 'amd64', ext = '';
  if (ua.indexOf('Windows') >= 0) { os = 'windows'; ext = '.exe'; }
  else if (ua.indexOf('Mac') >= 0 || ua.indexOf('Darwin') >= 0) { os = 'darwin'; }
  else if (ua.indexOf('Linux') >= 0) { os = 'linux'; }
  if (ua.indexOf('ARM64') >= 0 || ua.indexOf('aarch64') >= 0 || ua.indexOf('arm64') >= 0) arch = 'arm64';
  var names = {
    linux:   'sudodeckd-linux-' + arch,
    darwin:  'sudodeckd-darwin-' + arch,
    windows: 'sudodeckd-windows-' + arch + ext,
  };
  var osLabels = { linux: 'Linux', darwin: 'macOS', windows: 'Windows' };
  var installCmds = {
    linux:   './sudodeckd --install',
    darwin:  './sudodeckd-darwin-' + arch + ' --install',
    windows: 'sudodeckd-windows-' + arch + ext + ' --install',
  };
  return { os: os, arch: arch, file: names[os] || names.linux, osLabel: osLabels[os] || 'Linux', installCmd: installCmds[os] || installCmds.linux };
})();

function daemonShow(show) {
  var el = document.getElementById('daemonSetup');
  el.style.display = show ? '' : 'none';
  if (!show) return;
  document.getElementById('daemonOs').textContent = daemonInfo.osLabel;
  document.getElementById('daemonFileName').textContent = daemonInfo.file;
  var log = document.getElementById('daemonLog');
  log.textContent =
    'Detected OS: ' + daemonInfo.osLabel + ' (' + daemonInfo.arch + ')\n' +
    'Download: ' + daemonInfo.file + '\n' +
    '\n' +
    'After downloading:\n' +
    '  1. chmod +x ' + daemonInfo.file + '   (macOS/Linux only)\n' +
    '  2. ' + daemonInfo.installCmd + '\n' +
    '\n' +
    'The daemon will auto-start on every login. Plug in the CYD and tap a button to test.';
}

function daemonDownload() {
  var root = 'https://github.com/sudobreakstuff/sudodeck/releases/latest/download';
  var url = root + '/' + daemonInfo.file;
  var a = document.createElement('a');
  a.href = url;
  a.download = daemonInfo.file;
  a.click();
  fwLog('Downloading ' + daemonInfo.file + ' from GitHub releases...');
}

async function fwVersionChanged() {
  var sel = document.getElementById('fwVersion');
  if (!sel.value) {
    document.getElementById('btnFwFlash').disabled = true;
    fwFileData = null;
    return;
  }
  var label = sel.options[sel.selectedIndex].textContent;
  fwLog('Downloading ' + label + '...');
  try {
    var r = await fetch(fwBaseUrl() + '/' + sel.value);
    fwFileData = await r.arrayBuffer();
    fwFlashAddr = parseInt(sel.options[sel.selectedIndex].dataset.address);
    fwLog('Ready: ' + label + ' (' + (fwFileData.byteLength / 1024 / 1024).toFixed(1) + ' MB)');
    document.getElementById('btnFwFlash').disabled = false;
    document.getElementById('btnFwFlashLabel').textContent = 'Custom .bin';
  } catch (e) {
    fwLog('Download failed: ' + e.message);
  }
}

var fwPort = null;

async function fwConnect() {
  if (fwLoader) { fwLog('Already connected.'); return; }
  if (!('serial' in navigator)) { fwLog('Web Serial not available (Chrome/Edge required)'); return; }
  tryReleaseDaemon();
  if (fwPort) { try { fwPort.close(); } catch (e) {} fwPort = null; }
  fwTransport = null; fwLoader = null;
  try {
    fwLog('Loading esptool-js...');
    var esptool = await import('https://unpkg.com/esptool-js@0.6.0/bundle.js');
    var ESPLoader = esptool.ESPLoader;
    var Transport = esptool.Transport;
    fwLog('Library loaded. Requesting serial port...');
    fwLog('Put CYD in flash mode (hold BOOT, tap RESET, release BOOT), then select the port.');
    fwPort = await navigator.serial.requestPort();
    fwLog('Opening port...');
    var term = {
      clean: function() {},
      writeLine: function(d) { fwLog('> ' + d); },
      write: function(d) {},
    };
    fwTransport = new Transport(fwPort, false);
    fwLoader = new ESPLoader({ transport: fwTransport, baudrate: 115200, terminal: term });
    var chipName = await fwLoader.main();
    fwLog('Connected to: ' + chipName);
    fwSetStatus('Connected');
    document.getElementById('btnFwConnect').disabled = true;
    document.getElementById('btnFwConnect').textContent = 'Connected';
    document.getElementById('btnFwBackup').disabled = false;
    fwLog('Ready. You can backup or flash firmware.');
    fwBar(100);
  } catch (e) {
    fwLog('Connection failed: ' + e.message);
    fwSetStatus('Error');
    fwLoader = null; fwTransport = null;
    if (fwPort) { try { fwPort.close(); } catch (e2) {} fwPort = null; }
  }
}

async function fwBackup() {
  if (!fwLoader) { fwLog('Connect first'); return; }
  var btn = document.getElementById('btnFwBackup');
  btn.disabled = true; btn.textContent = 'Reading…';
  fwLog('Reading full flash (4MB). This will take a minute…');
  fwBar(0);
  try {
    var data = await fwLoader.readFlash(0, 0x400000, function(pkt, prog, total) {
      fwBar((prog / total) * 100);
    });
    var blob = new Blob([data], { type: 'application/octet-stream' });
    var url = URL.createObjectURL(blob);
    var a = document.createElement('a');
    a.href = url; a.download = 'sudodeck-backup-' + Date.now() + '.bin';
    a.click();
    URL.revokeObjectURL(url);
    fwLog('Backup saved! Size: ' + (data.byteLength / 1024 / 1024).toFixed(1) + ' MB');
    fwBar(100);
  } catch (e) {
    fwLog('Backup failed: ' + e.message);
  }
  btn.disabled = false; btn.textContent = 'Backup Firmware';
}

function fwFileSelected() {
  var f = document.getElementById('btnFwFile').files[0];
  if (!f) return;
  var r = new FileReader();
  r.onload = function(ev) {
    fwFileData = ev.target.result;
    fwFlashAddr = 0;
    fwLog('Selected: ' + f.name + ' (' + (fwFileData.byteLength / 1024).toFixed(0) + ' KB)');
    document.getElementById('btnFwFlash').disabled = false;
    document.getElementById('btnFwFlashLabel').textContent = f.name;
    document.getElementById('fwVersion').value = '';
  };
  r.readAsArrayBuffer(f);
}

async function fwFlash() {
  if (!fwLoader) { fwLog('Connect first'); return; }
  if (!fwFileData) { fwLog('Select or choose a firmware version first'); return; }
  var btn = document.getElementById('btnFwFlash');
  btn.disabled = true; btn.textContent = 'Flashing…';
  document.getElementById('btnFwBackup').disabled = true;
  fwLog('Flashing firmware at 0x' + fwFlashAddr.toString(16) + '… DO NOT DISCONNECT!');
  fwBar(0);
  try {
    var opts = {
      fileArray: [{ data: new Uint8Array(fwFileData), address: fwFlashAddr }],
      flashSize: '4MB',
      flashMode: 'dio',
      flashFreq: '40m',
      eraseAll: false,
      compress: true,
      verify: true,
      reportProgress: function(idx, written, total) {
        fwBar((written / total) * 100);
      },
    };
    var result = await fwLoader.writeFlash(opts);
    fwLog('Flash complete! Resetting...');
    try { await fwLoader.after('hard_reset'); } catch (e) {}
    fwLog('Done! Reconnect USB. The CYD will boot the new firmware.');
    fwBar(100);
  } catch (e) {
    fwLog('Flash failed: ' + e.message);
  }
  btn.disabled = false; btn.textContent = 'Flash';
}
