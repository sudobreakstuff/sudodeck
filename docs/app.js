
const iv = new IntersectionObserver(function(es) {
  es.forEach(function(e) { if (e.isIntersecting) e.target.classList.add('in'); });
}, { threshold: 0.1 });
document.querySelectorAll('.rv').forEach(function(el) { iv.observe(el); });
document.querySelectorAll('.section-reveal').forEach(function(el) { iv.observe(el); });

var dp = null, dr = null, dw = null, cd = null, sp = 0, sb = -1, __srl_buf = '', __busy = false;

function dc() {
  return {
    name: "SudoDeck", grid: { cols: 4, rows: 3 },
    wifi: { ssid: "", password: "" },
    saver: { timeout: 30, sleep: 60, mode: 0 },
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
async function sc(c, t) { if (!dw) throw new Error('nc'); if(!dr) throw new Error('nr'); console.log('sc send:',JSON.stringify(c)); await dw.write(new TextEncoder().encode(JSON.stringify(c)+'\n')); await new Promise(function(r) { setTimeout(r, 200); }); var s = await srl(t); console.log('sc recv:',s); if (!s) throw new Error('no response'); return JSON.parse(s); }
document.getElementById('btnConnect').addEventListener('click', async function() {
  try {
    if (!('serial' in navigator)) { tm('Web Serial needs Chrome/Edge', 'ng'); return; }
    var p = await navigator.serial.requestPort(); await p.open({baudRate:115200});
    dp = p; dr = p.readable.getReader(); dw = p.writable.getWriter(); ss(true); tm('Connected');
    try { var r = await sc({cmd:'get_info'}); tm(r.name+' v'+(r.version||'?')); } catch(e) {}
  } catch(e) { if(e.message!=='The device has been lost.') tm('Connection failed', 'ng'); ss(false); }
});
window.addEventListener('beforeunload', async function() {
  if(dr) { try { await dr.cancel(); } catch(e){} } if(dw) { try { await dw.close(); } catch(e){} } if(dp) { try { await dp.close(); } catch(e){} }
});
document.getElementById('btnRead').addEventListener('click', async function() {
  if(!dw) { tm('Connect first', 'ng'); return; }
  try { var r = await sc({cmd:'get_config'}); if(r.config) { cd=r.config; sp=0; sb=-1; ra(); tm('Config loaded'); } } catch(e) { console.error('Read failed:', e); tm('Read failed', 'ng'); }
});
document.getElementById('btnWrite').addEventListener('click', async function() {
  if(!dw) { tm('Connect first', 'ng'); return; }
  if(__busy) return; __busy = true; var btn = this; btn.disabled = true;
  syncWifi(); syncSaver(); syncWidgets();
  try { var r = await sc({cmd:'set_config',config:cd}, 15000); if(r && r.error) { console.log('Write ERR:',r); var s = r.got||''; tm('CYD: '+r.error+' (len:'+r.len+') "'+s.substring(0,60)+'"', 'ng'); return; } tm('Written to CYD'); } catch(e) { console.error('Write failed:', e); tm('Write failed: '+(e.message||e), 'ng'); } finally { __busy = false; btn.disabled = false; }
});
document.getElementById('btnExport').addEventListener('click', function() {
  var b = new Blob([JSON.stringify(cd,null,2)], {type:'application/json'});
  var u = URL.createObjectURL(b); var a = document.createElement('a'); a.href=u; a.download='sudodeck-config.json'; a.click(); URL.revokeObjectURL(u); tm('Exported');
});
document.getElementById('btnReset').addEventListener('click', async function() {
  if(!confirm('Factory reset?')) return;
  if(dw) { try { await sc({cmd:'factory_reset'}); var r = await sc({cmd:'get_config'}); if(r.config) cd=r.config; } catch(e) { tm('Reset failed','ng'); return; } }
  else { cd=dc(); }
  sp=0; sb=-1; ra(); tm('Reset done');
});
document.getElementById('fileInput').addEventListener('change', function(e) {
  var f = e.target.files[0]; if(!f) return;
  var r = new FileReader();
  r.onload = function(ev) {
    try { var d = JSON.parse(ev.target.result); if(!d.pages||!d.grid) throw Error('bad'); cd=d; sp=0; sb=-1; ra(); tm('Imported'); } catch(err) { tm('Invalid file','ng'); }
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
  document.getElementById('lc').value=cd.grid.cols;
  document.getElementById('lr').value=cd.grid.rows;
  document.getElementById('lp').value=cd.pages.length;
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
document.getElementById('bal').addEventListener('click', function() {
  var c=Math.max(1,Math.min(6,parseInt(document.getElementById('lc').value)||4));
  var r=Math.max(1,Math.min(5,parseInt(document.getElementById('lr').value)||3));
  var pc=Math.max(1,Math.min(12,parseInt(document.getElementById('lp').value)||1));
  cd.grid.cols=c; cd.grid.rows=r; var tb=c*r;
  while(cd.pages.length<pc) { var bt=[]; for(var i=0;i<tb;i++) bt.push({label:'',color:'#16213E',action:{type:'key',value:''}}); cd.pages.push({name:'Page '+(cd.pages.length+1),buttons:bt}); }
  while(cd.pages.length>pc) cd.pages.pop();
  for(var i=0;i<cd.pages.length;i++) { while(cd.pages[i].buttons.length<tb) cd.pages[i].buttons.push({label:'',color:'#16213E',action:{type:'key',value:''}}); }
  syncWifi(); syncSaver();
  sp=0; sb=-1; ra(); tm('Layout updated');
});
function ra() { rpb(); rg(); ul(); syncWidgets(); }
function rpb() {
  var b = document.getElementById('pBar'); b.innerHTML = '';
  if(!cd) return;
  for(var i=0;i<cd.pages.length;i++) {
    var t = document.createElement('span');
    t.className = 'p-tab' + (i===sp?' on':'');
    t.textContent = cd.pages[i].name || ('Pg'+(i+1));
    t.addEventListener('click', function(idx) { return function() { sp=idx; sb=-1; ra(); }; }(i));
    t.addEventListener('dblclick', function(idx) { return function() { var n=prompt('Page name:',cd.pages[idx].name); if(n!==null) { cd.pages[idx].name=n; ra(); } }; }(i));
    b.appendChild(t);
  }
}
function rg() {
  var g = document.getElementById('gp');
  if(!cd) return;
  var cols=cd.grid.cols, rows=cd.grid.rows;
  g.style.gridTemplateColumns = 'repeat('+cols+',1fr)';
  var p = cd.pages[sp]; if(!p) return;
  g.innerHTML = '';
  for(var i=0;i<cols*rows;i++) {
    var b = document.createElement('div');
    b.className = 'gb';
    if(i>=p.buttons.length||!p.buttons[i]||!p.buttons[i].label) b.classList.add('em');
    if(i===sb) b.classList.add('sel');
    var d = p.buttons[i]||{label:'',color:'#16213E'};
    b.style.backgroundColor = d.color||'#16213E';
    b.innerHTML = '<span class="ix">'+(i+1)+'</span>'+(d.label||'...');
    b.addEventListener('click', function(idx) { return function() { sb=idx; rg(); se(idx); }; }(i));
    g.appendChild(b);
  }
}
function se(idx) {
  var p = document.getElementById('ep');
  var pg = cd.pages[sp];
  if(!pg||idx>=pg.buttons.length) { p.style.display='none'; return; }
  p.style.display='flex';
  var b = pg.buttons[idx];
  document.getElementById('el').value = b.label||'';
  document.getElementById('ec').value = b.color||'#16213E';
  document.getElementById('eat').value = b.action?b.action.type:'key';
  uaf();
}
function uaf() {
  var t = document.getElementById('eat').value;
  var c = document.getElementById('af');
  var pg = cd.pages[sp];
  var b = pg?pg.buttons[sb]:null;
  var a = b?b.action:{type:'key',value:''};
  var h = '';
  if(t==='key') {
    h = '<div class="er"><label>Key</label><input type="text" id="ekv" value="'+es(a.value||'')+'" placeholder="ENTER, F1, a"></div>';
  } else if(t==='combo') {
    h = '<div class="er"><label>Mod</label><select id="ecm">'+
      ['CTRL','ALT','SHIFT','GUI','CTRL_SHIFT','CTRL_ALT','CTRL_GUI','ALT_SHIFT','ALT_GUI','SHIFT_GUI','CTRL_ALT_SHIFT','CTRL_ALT_GUI','CTRL_SHIFT_GUI','ALT_SHIFT_GUI','CTRL_ALT_SHIFT_GUI'].map(function(m){return '<option value="'+m+'"'+(a.mod===m?' selected':'')+'>'+m.replace(/_/g,'+')+'</option>';}).join('')+
      '</select><label>Key</label><input type="text" id="eck" value="'+es(a.key||'')+'" placeholder="c, s, v"></div>';
  } else if(t==='text') {
    h = '<div class="er"><label>Text</label><input type="text" id="etv" value="'+es(a.value||'')+'" placeholder="Text to type"></div>';
  } else if(t==='delay') {
    h = '<div class="er"><label>Delay</label><input type="number" id="edv" value="'+(a.value||100)+'" min="1" max="10000"> <span style="font-size:0.7rem;color:var(--dim)">ms</span></div>';
  } else if(t==='app') {
    h = '<div class="er"><label>OS</label><select id="aos">'+
      ['windows','macos','linux'].map(function(o){return '<option value="'+o+'"'+(a.os===o?' selected':'')+'>'+o.charAt(0).toUpperCase()+o.slice(1)+'</option>';}).join('')+
      '</select></div>'+
      '<div class="er"><label>App name</label><input type="text" id="anv" value="'+es(a.name||'')+'" placeholder="Calculator"> <span style="font-size:0.65rem;color:var(--dim)">use the display name, like Calculator or Notepad</span></div>';
  } else if(t==='macro') {
    h = '<div class="ab"><div style="display:flex;justify-content:space-between;align-items:center;margin-bottom:6px;"><span style="color:var(--acc);font-size:0.7rem;">STEPS</span><button class="btn" id="bas">+ Add</button></div><div id="mss"></div></div>';
    setTimeout(function() {
      rms(a.steps||[]);
      document.getElementById('bas').addEventListener('click', function() {
        var s = gaf().steps||[]; s.push({type:'key',value:''});
        cd.pages[sp].buttons[sb].action = {type:'macro',steps:s};
        uaf();
      });
    }, 50);
  }
  c.innerHTML = h;
}
function rms(steps) {
  var c = document.getElementById('mss'); if(!c) return;
  c.innerHTML = '';
  steps = steps||[];
  var mods = ['CTRL','ALT','SHIFT','GUI','CTRL_SHIFT','CTRL_ALT','CTRL_GUI','ALT_SHIFT','ALT_GUI','SHIFT_GUI','CTRL_ALT_SHIFT','CTRL_ALT_GUI','CTRL_SHIFT_GUI','ALT_SHIFT_GUI','CTRL_ALT_SHIFT_GUI'];
  steps.forEach(function(s,i) {
    var d = document.createElement('div'); d.className = 'ms';
    var inner = '<span style="color:var(--dim);font-size:0.6rem;min-width:18px;font-family:var(--mono)">#'+(i+1)+'</span>'+
      '<select class="mt" data-idx="'+i+'">'+['key','combo','text','delay'].map(function(t){return '<option value="'+t+'"'+(s.type===t?' selected':'')+'>'+t.charAt(0).toUpperCase()+t.slice(1)+'</option>';}).join('')+'</select>';
    if(s.type==='combo') {
      inner += '<select class="mm" data-idx="'+i+'" style="flex:0 1 auto;max-width:100px;">'+
        mods.map(function(m){return '<option value="'+m+'"'+(s.mod===m?' selected':'')+'>'+m.replace(/_/g,'+')+'</option>';}).join('')+
        '</select>'+
        '<input type="text" class="mv" value="'+es(s.key||'')+'" data-idx="'+i+'" placeholder="key (c, v...)" style="flex:1">';
    } else if(s.type==='delay') {
      inner += '<input type="number" class="mv" value="'+(s.value||100)+'" data-idx="'+i+'" min="1" max="10000" style="width:80px;">'+
        '<span style="color:var(--dim);font-size:0.65rem;">ms</span>';
    } else {
      inner += '<input type="text" class="mv" value="'+es(s.value||'')+'" data-idx="'+i+'" placeholder="value" style="flex:1">';
    }
    inner += '<button class="btn dan md" data-idx="'+i+'" style="padding:3px 8px;font-size:0.65rem;">X</button>';
    d.innerHTML = inner;
    c.appendChild(d);
  });
  c.querySelectorAll('.mt').forEach(function(s) {
    s.addEventListener('change', function(e) {
      var idx = parseInt(e.target.dataset.idx);
      if(cd.pages[sp].buttons[sb].action.steps) { cd.pages[sp].buttons[sb].action.steps[idx].type = e.target.value; }
      rms(cd.pages[sp].buttons[sb].action.steps);
    });
  });
  c.querySelectorAll('.mv').forEach(function(i) {
    i.addEventListener('input', function(e) {
      var idx = parseInt(e.target.dataset.idx);
      if(cd.pages[sp].buttons[sb].action.steps) {
        var s2 = cd.pages[sp].buttons[sb].action.steps[idx];
        if(s2.type==='key'||s2.type==='text') s2.value = e.target.value;
        else if(s2.type==='combo') s2.key = e.target.value;
        else if(s2.type==='delay') s2.value = parseInt(e.target.value)||100;
      }
    });
  });
  c.querySelectorAll('.mm').forEach(function(s) {
    s.addEventListener('change', function(e) {
      var idx = parseInt(e.target.dataset.idx);
      if(cd.pages[sp].buttons[sb].action.steps) { cd.pages[sp].buttons[sb].action.steps[idx].mod = e.target.value; }
    });
  });
  c.querySelectorAll('.md').forEach(function(b) {
    b.addEventListener('click', function(e) {
      var idx = parseInt(e.target.dataset.idx);
      if(cd.pages[sp].buttons[sb].action.steps) { cd.pages[sp].buttons[sb].action.steps.splice(idx,1); rms(cd.pages[sp].buttons[sb].action.steps); }
    });
  });
}
function gaf() {
  var t = document.getElementById('eat').value;
  if(t==='key') return {type:'key',value:document.getElementById('ekv')?.value||''};
  if(t==='combo') return {type:'combo',mod:document.getElementById('ecm')?.value||'CTRL',key:document.getElementById('eck')?.value||''};
  if(t==='text') return {type:'text',value:document.getElementById('etv')?.value||''};
  if(t==='delay') return {type:'delay',value:parseInt(document.getElementById('edv')?.value)||100};
  if(t==='app') return {type:'app',os:document.getElementById('aos')?.value||'windows',name:document.getElementById('anv')?.value||''};
  if(t==='macro') { return {type:'macro',steps:cd.pages[sp].buttons[sb].action?.steps||[]}; }
  return {type:'key',value:''};
}
document.getElementById('eat').addEventListener('change', function() {
  var pg = cd.pages[sp];
  if(pg&&sb>=0) {
    var nt = document.getElementById('eat').value;
    var na = {type:nt};
    if(nt==='key'||nt==='text') na.value='';
    if(nt==='combo') { na.mod='CTRL'; na.key=''; }
    if(nt==='delay') na.value=100;
    if(nt==='app') { na.os='windows'; na.name=''; }
    if(nt==='macro') na.steps=[];
    pg.buttons[sb].action = na;
  }
  uaf();
});
document.getElementById('bae').addEventListener('click', function() {
  var pg = cd.pages[sp]; if(!pg||sb<0) return;
  pg.buttons[sb].label = document.getElementById('el').value;
  pg.buttons[sb].color = document.getElementById('ec').value;
  pg.buttons[sb].action = gaf();
  document.getElementById('ep').style.display = 'none';
  ra(); tm('Button updated');
});
document.getElementById('bce').addEventListener('click', function() {
  document.getElementById('ep').style.display = 'none';
  sb = -1; rg();
});
function es(s) { return (s||'').replace(/&/g,'&amp;').replace(/</g,'&lt;').replace(/>/g,'&gt;').replace(/"/g,'&#34;'); }

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

// Screensaver

document.getElementById('saverToggle').addEventListener('click', function() {
  var b = document.getElementById('saverBody');
  b.classList.toggle('open');
});
async function saverSync() {
  if(!dw) return;
  try { var r = await sc({cmd:'get_saver'}); 
    document.getElementById('saverTimeout').value = r.timeout || 30;
    document.getElementById('saverSleep').value = r.sleep != null ? r.sleep : 60;
    document.getElementById('saverHint').textContent = r.timeout + 's / sleep ' + (r.sleep || 'off');
    if(r.mode) document.getElementById('saverMode').value = r.mode;
    if(cd && cd.saver) { cd.saver.timeout=r.timeout||30; cd.saver.sleep=r.sleep||60; cd.saver.mode=modeToInt(r.mode||'matrix'); }
  } catch(e) {}
}
document.getElementById('btnSaverSet').addEventListener('click', async function() {
  if(!dw) { tm('Connect first', 'ng'); return; }
  var t = parseInt(document.getElementById('saverTimeout').value) || 30;
  var s = parseInt(document.getElementById('saverSleep').value) || 0;
  if(t < 5) t = 5; if(t > 600) t = 600;
  if(s < 0) s = 0; if(s > 600) s = 600;
  try { await sc({cmd:'set_saver',timeout:t,sleep:s}); cd.saver.timeout=t; cd.saver.sleep=s; tm('Timeout ' + t + 's, sleep ' + (s||'off')); saverSync(); } catch(e) { tm('Failed', 'ng'); }
});

document.getElementById('btnSaverModeSet').addEventListener('click', async function() {
  if(!dw) { tm('Connect first', 'ng'); return; }
  var m = document.getElementById('saverMode').value;
  try { var r = await sc({cmd:'set_saver_mode',mode:m}); if(r&&r.error) { tm('Mode error: '+r.error,'ng'); return; } cd.saver.mode=modeToInt(m); tm('Saver mode: '+m); saverSync(); } catch(e) { tm('Mode command failed','ng'); }
});


// Patch connect to also sync saver status
(function() {
  var btn = document.getElementById('btnConnect');
  var clone = btn.cloneNode(true);
  btn.parentNode.replaceChild(clone, btn);
  clone.addEventListener('click', async function() {
    try {
      if (!('serial' in navigator)) { tm('Web Serial needs Chrome/Edge', 'ng'); return; }
      var p = await navigator.serial.requestPort(); await p.open({baudRate:115200});
      dp = p; dr = p.readable.getReader(); dw = p.writable.getWriter(); ss(true); tm('Connected');
      try { var r = await sc({cmd:'get_info'}); tm(r.name+' v'+(r.version||'?')); } catch(e) {}
      setTimeout(saverSync, 500);
    } catch(e) { if(e.message!=='The device has been lost.') tm('Connection failed', 'ng'); ss(false); }
  });
})();

document.getElementById('widgetToggle').addEventListener('click', function() {
  var b = document.getElementById('widgetBody');
  b.classList.toggle('open');
});
document.getElementById('btnWidgetAdd').addEventListener('click', function() {
  if(!cd) return;
  if(!cd.widgets) cd.widgets=[];
  cd.widgets.push({label:'',url:'',path:'',format:'{value}',interval:60});
  syncWidgets();
  var list = document.getElementById('widgetList');
  var last = list.lastElementChild;
  if(last) {
    var ed = last.querySelector('.wc-edit');
    if(ed) ed.classList.add('open');
  }
});
document.getElementById('widgetList').addEventListener('click', function(e) {
  var btn = e.target.closest('button');
  if(!btn) return;
  var i = parseInt(btn.getAttribute('data-i'));
  if(isNaN(i) || !cd || !cd.widgets || i >= cd.widgets.length) return;
  if(btn.classList.contains('wc-edit-btn')) {
    var ed = document.getElementById('wcEdit'+i);
    if(ed) ed.classList.toggle('open');
  } else if(btn.classList.contains('wc-rm-btn')) {
    cd.widgets.splice(i,1);
    syncWidgets();
  } else if(btn.classList.contains('wc-save-btn')) {
    var ed = document.getElementById('wcEdit'+i);
    if(!ed) return;
    var w = cd.widgets[i];
    ed.querySelectorAll('.wc-fi').forEach(function(inp) {
      var f = inp.getAttribute('data-f');
      if(f === 'interval') w[f] = parseInt(inp.value) || 60;
      else w[f] = inp.value;
    });
    syncWidgets();
  } else if(btn.classList.contains('wc-test-btn')) {
    var w = cd.widgets[i];
    var te = document.getElementById('wcTest'+i);
    if(!te || !w.url) { if(te) { te.textContent='no url'; te.className='wc-test ng'; } return; }
    te.textContent='testing...'; te.className='wc-test';
    fetch(w.url, {signal:AbortSignal.timeout(10000)})
      .then(function(r) { if(!r.ok) throw Error('HTTP '+r.status); return r.text(); })
      .then(function(body) {
        try {
          var pd = JSON.parse(body);
          var val = pd;
          var segs = (w.path||'').split('.');
          for(var k=0;k<segs.length;k++) {
            var s = segs[k];
            var br = s.indexOf('[');
            if(br>=0) { var idx = parseInt(s.substring(br+1)); s = s.substring(0,br); if(s) val=val[s]; val=val[idx]; }
            else if(s) val=val[s];
            if(val===undefined||val===null) { val=undefined; break; }
          }
          if(val===undefined) te.textContent='path not found';
          else {
            var fmt = w.format||'{value}';
            te.textContent = fmt.replace('{value}', String(val));
          }
          te.className='wc-test ok';
        } catch(e) { te.textContent='parse error: '+e.message; te.className='wc-test ng'; }
      })
      .catch(function(e) { te.textContent='fetch error: '+(e.message||e); te.className='wc-test ng'; });
  }
});

ic(); ra();

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
  return window.location.pathname.replace(/\/[^/]*$/, '') + '/assets/firmware';
}
async function fwLoadManifest() {
  try {
    var r = await fetch(fwBaseUrl() + '/manifest.json');
    if (!r.ok) { fwLog('HTTP ' + r.status); return; }
    fwManifest = await r.json();
    var sel = document.getElementById('fwVersion');
    fwManifest.versions.forEach(function(v) {
      var opt = document.createElement('option');
      opt.value = v.file;
      opt.textContent = v.label;
      opt.dataset.address = v.address;
      sel.appendChild(opt);
    });
    fwLog('Firmware versions loaded.');
  } catch(e) {
    fwLog('Could not load firmware list.');
  }
}
fwLoadManifest();

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
  } catch(e) {
    fwLog('Download failed: ' + e.message);
  }
}

var fwPort = null;

async function fwConnect() {
  if (fwLoader) { fwLog('Already connected.'); return; }
  if (!('serial' in navigator)) { fwLog('Web Serial not available (Chrome/Edge required)'); return; }

  // Close stale port if any (from a previous failed attempt)
  if (fwPort) { try { fwPort.close(); } catch(e) {} fwPort = null; }
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
  } catch(e) {
    fwLog('Connection failed: ' + e.message);
    fwSetStatus('Error');
    fwLoader = null; fwTransport = null;
    if (fwPort) { try { fwPort.close(); } catch(e2) {} fwPort = null; }
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
  } catch(e) {
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
    fwLog('Selected: ' + f.name + ' (' + (fwFileData.byteLength/1024).toFixed(0) + ' KB)');
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
    try { await fwLoader.after('hard_reset'); } catch(e) {}
    fwLog('Done! Reconnect USB. The CYD will boot the new firmware.');
    fwBar(100);
  } catch(e) {
    fwLog('Flash failed: ' + e.message);
  }
  btn.disabled = false; btn.textContent = 'Flash';

}

