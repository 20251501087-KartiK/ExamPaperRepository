"use strict";
/* PaperVault — browser port of endsem_terminal.cpp.
   Same classes (ExamPaper, User > Student/Faculty/Admin, PaperManager), same sample data,
   same login IDs and the same Pending -> Verified/Rejected workflow. Data persists in localStorage. */

const $ = (s, r = document) => r.querySelector(s), $$ = (s, r = document) => [...r.querySelectorAll(s)];
const esc = s => String(s).replace(/[&<>"']/g, c => ({ "&": "&amp;", "<": "&lt;", ">": "&gt;", '"': "&quot;", "'": "&#39;" }[c]));

/* ---------- Backend port ---------- */
class ExamPaper {                       // encapsulation: data is private, read through getters
  #d;
  constructor(id, code, name, branch, sem, type, year, file, by, status = "Pending") {
    this.#d = { id, code, name, branch, sem, type, year, file, by, status };
  }
  static { for (const k of ["id", "code", "name", "branch", "sem", "type", "year", "file", "by", "status"]) Object.defineProperty(this.prototype, k, { get() { return this.#d[k]; } }); }
  setStatus(s) { this.#d.status = s; }
  setExamDetails(type, year) { this.#d.type = type; this.#d.year = year; }
  isOlderThan(o) { return this.year < o.year; }          // operator<
  toJSON() { return { ...this.#d }; }
  static from(o) { return new ExamPaper(o.id, o.code, o.name, o.branch, o.sem, o.type, o.year, o.file, o.by, o.status); }
}
class User {                            // abstraction: User cannot be created directly
  #pw;
  constructor(id, name, email, pw) {
    if (new.target === User) throw new TypeError("User is abstract");
    Object.assign(this, { id, name, email }); this.#pw = pw;
  }
  login(pw) { return pw === this.#pw; }
  get role() { throw new Error("role must be overridden"); }
}
class Student extends User { downloads = []; saved = []; get role() { return "Student"; } }
class Faculty extends User { get role() { return "Faculty"; } }
class Admin extends User { get role() { return "Admin"; } }

class PaperManager {
  constructor() {
    this.nextId = 1001;
    this.papers = [
      ["CS301", "Data Structures", "CSE", 3, "Mid Semester", 2025, "DS_MidSem_2025.pdf", "Verified"],
      ["CS201", "Object Oriented Programming", "CSE", 4, "Mid Semester", 2026, "OOP_MidSem_2026.pdf", "Verified"],
      ["CS202", "Database Management System", "CSE", 4, "End Semester", 2025, "DBMS_EndSem_2025.pdf", "Pending"],
      ["EC201", "Digital Logic", "ECE", 3, "Mid Semester", 2026, "DL_MidSem_2026.pdf", "Verified"]
    ].map(([c, n, b, s, t, y, f, st]) => new ExamPaper(this.nextId++, c, n, b, s, t, y, f, "F014", st));
  }
  findById(id) { return this.papers.find(p => p.id === +id) || null; }
  addPaper(p) { this.papers.push(p); }
  generateId() { return this.nextId++; }
  search({ branch = "Any", semester = 0, subject = "Any", year = 0, type = "Any" } = {}) {
    return this.papers.filter(p => (branch === "Any" || p.branch === branch) && (!semester || p.sem === semester) &&
      (subject === "Any" || p.name === subject) && (!year || p.year === year) && (type === "Any" || p.type === type));
  }
  sortByYear() { this.papers.sort((a, b) => a.year - b.year); }
}

const KEY = "papervault.v1", manager = new PaperManager();
const users = {
  Student: new Student("S1023", "Riya Sharma", "riya.sharma@college.edu", "1234"),
  Faculty: new Faculty("F014", "Dr. A. Verma", "verma@college.edu", "1234"),
  Admin: new Admin("A002", "Mr. R. Singh", "singh@college.edu", "1234")
};
try {
  const s = JSON.parse(localStorage.getItem(KEY));
  if (s && Array.isArray(s.papers)) {
    manager.papers = s.papers.map(ExamPaper.from); manager.nextId = s.nextId;
    users.Student.downloads = s.downloads || []; users.Student.saved = s.saved || [];
  }
} catch { /* first visit or storage blocked: sample data is used */ }
const persist = () => { try { localStorage.setItem(KEY, JSON.stringify({ papers: manager.papers, nextId: manager.nextId, downloads: users.Student.downloads, saved: users.Student.saved })); } catch { } };

/* ---------- UI helpers ---------- */
let me = null, role = "Student", tab = "", editId = null, limit = 6, tt;
const BR = ["CSE", "IT", "ECE"], EX = ["Mid Semester", "End Semester"];
const toast = m => { const t = $("#toast"); t.textContent = m; t.classList.add("show"); clearTimeout(tt); tt = setTimeout(() => t.classList.remove("show"), 2800); };
const openM = id => { $(id).hidden = false; };
const closeAll = () => $$(".modal-backdrop").forEach(m => m.hidden = true);
const badge = s => `<span class="st st-${s.toLowerCase().replace(/\s+/g, "-")}">${esc(s)}</span>`;
const btn = (a, id, t) => `<button class="mini" data-act="${a}" data-id="${id}">${t}</button>`;
const opts = a => a.map(x => `<option>${x}</option>`).join("");

/* ---------- Public library ---------- */
function visible() {
  const q = $("#filterQuery").value.trim().toLowerCase(), sort = $("#sortFilter").value;
  const r = manager.search({ branch: $("#branchFilter").value || "Any", semester: +$("#semesterFilter").value, type: $("#examFilter").value || "Any" })
    .filter(p => p.status === "Verified" && (!q || `${p.name} ${p.code} ${p.type} ${p.branch} semester ${p.sem} ${p.year}`.toLowerCase().includes(q)));
  return sort === "title" ? r.sort((a, b) => a.name.localeCompare(b.name)) : r.sort((a, b) => b.year - a.year || b.id - a.id);
}
function renderGrid() {
  const all = visible(), list = all.slice(0, limit);
  $("#paperGrid").innerHTML = list.map(p => `<article class="paper-card"><div class="card-top"><span class="file-badge">PDF</span><span class="year-badge">${p.year}</span></div>
    <h3>${esc(p.name)}</h3><div class="card-meta">${esc(p.code)} · ${esc(p.type)}<br>${esc(p.branch)} · Semester ${p.sem}</div>
    <div class="card-tags"><span>${esc(p.branch)}</span><span>Sem ${p.sem}</span><span>${esc(p.type)}</span></div>
    <div class="card-bottom"><span class="downloads">Paper #${p.id}</span><button class="view-paper" data-act="view" data-id="${p.id}">View details <span>→</span></button></div></article>`).join("");
  $("#emptyState").hidden = all.length > 0;
  $("#resultCount").textContent = `Showing ${list.length} of ${all.length} verified paper${all.length === 1 ? "" : "s"}`;
  $("#loadMore").hidden = all.length <= limit;
  $("#libNote").textContent = "Only admin-verified papers appear here";
}
function showPaper(id) {
  const p = manager.findById(id); if (!p) return;
  const row = (k, v) => `<b>${k}</b> ${esc(v)}<br>`;
  $("#modalContent").innerHTML = `<h2 class="detail-title">${esc(p.name)}</h2><div class="detail-meta">${row("Paper ID", p.id)}${row("Subject code", p.code)}${row("Branch", p.branch)}${row("Semester", p.sem)}${row("Exam type", p.type)}${row("Year", p.year)}${row("File name", p.file)}${row("Uploaded by", p.by)}${row("Status", p.status)}</div>
    <div class="detail-preview">PDF<strong>${esc(p.file)}</strong><small>File storage is not connected in this version, so downloads are recorded but no PDF is served.</small></div>
    <div class="modal-actions">${me instanceof Student ? btn("download", p.id, "Download paper") + btn("save", p.id, "Save paper") : `<button class="button button-dark button-small" data-act="login">Log in as a student to download or save</button>`}</div>`;
  closeAll(); openM("#paperModal");
}

/* ---------- Login ---------- */
function syncRole() { $$("#roleTabs button").forEach(b => b.classList.toggle("on", b.dataset.role === role)); $("#loginId").value = users[role].id; }
function openLogin(r) { role = r || role; syncRole(); $("#loginPw").value = ""; $("#loginMsg").textContent = ""; closeAll(); openM("#loginModal"); setTimeout(() => $("#loginPw").focus(), 60); }
function contribute(e) {
  e.preventDefault();
  if (me instanceof Faculty) return openPortal("upload");
  if (me) return toast("Only faculty can upload papers.");
  toast("Log in as faculty to upload a paper."); openLogin("Faculty");
}

/* ---------- Dashboards (one per role, mirroring the C++ menus) ---------- */
const NAV = {
  Student: [["home", "Dashboard"], ["browse", "Search papers"], ["subjects", "Browse subjects"], ["downloads", "My downloads"], ["saved", "Saved papers"], ["profile", "My profile"]],
  Faculty: [["home", "Dashboard"], ["browse", "Search papers"], ["upload", "Upload paper"], ["mine", "My papers"], ["profile", "My profile"]],
  Admin: [["reports", "Reports"], ["verify", "Verify papers"], ["all", "Manage papers"], ["users", "Manage users"], ["subjects", "Manage subjects"], ["profile", "My profile"]]
};
const mine = () => manager.papers.filter(p => p.by === me.id), count = s => manager.papers.filter(p => p.status === s).length;
const byIds = ids => ids.map(i => manager.findById(i)).filter(Boolean);
const head = (t, e) => `<h2>${t}</h2>${e ? `<p class="modal-intro">${e}</p>` : ""}`;
const stats = a => `<div class="stat-row">${a.map(([l, n]) => `<div class="stat"><b>${n}</b><span>${l}</span></div>`).join("")}</div>`;
const grid = (h, r) => r.length ? `<div class="tbl-wrap"><table class="tbl"><thead><tr>${h.map(x => `<th>${x}</th>`).join("")}</tr></thead><tbody>${r.map(c => `<tr>${c.map(x => `<td>${x}</td>`).join("")}</tr>`).join("")}</tbody></table></div>` : `<p class="modal-intro">Nothing here yet.</p>`;
const pTbl = (l, acts) => grid(["ID", "Subject", "Exam", "Branch", "Sem", "Year", "Status", ...(acts ? [""] : [])],
  l.map(p => [p.id, esc(p.name), esc(p.type), esc(p.branch), p.sem, p.year, badge(p.status), ...(acts ? [`<div class="acts">${acts(p)}</div>`] : [])]));
const examSel = (v = "") => `<select name="type">${EX.map(x => `<option${x === v ? " selected" : ""}>${x}</option>`).join("")}</select>`;

const V = {
  home: () => me instanceof Student
    ? head(`Welcome, ${esc(me.name)}!`, "Search the library, then download or save the papers you need.") + stats([["Downloads", me.downloads.length], ["Saved papers", me.saved.length], ["Papers available", count("Verified")]])
    : head(`Welcome, ${esc(me.name)}!`, "Upload papers and keep your listings up to date.") + stats([["My papers", mine().length], ["Awaiting verification", mine().filter(p => p.status === "Pending").length], ["Verified", mine().filter(p => p.status === "Verified").length]]),
  subjects: () => head(me.role === "Admin" ? "Manage subjects" : "Browse subjects") + grid(["Code", "Subject", "Branch"], [...new Map(manager.papers.map(p => [p.code, p])).values()].map(p => [esc(p.code), esc(p.name), esc(p.branch)])),
  downloads: () => head("My downloads") + pTbl(byIds(me.downloads), p => btn("view", p.id, "View")),
  saved: () => head("Saved papers") + pTbl(byIds(me.saved), p => btn("view", p.id, "View")),
  profile: () => head("My profile") + grid(["Field", "Detail"], [["Role", me.role], ["User ID", esc(me.id)], ["Name", esc(me.name)], ["Email", esc(me.email)]]),
  upload: () => head("Upload a paper", "New papers stay Pending until an admin verifies them.") +
    `<form class="upload-form" data-form="upload"><div class="form-two"><label>Subject code<input name="code" required placeholder="CS201"></label><label>Subject name<input name="name" required placeholder="Object Oriented Programming"></label></div>
    <div class="form-two"><label>Branch<select name="branch">${opts(BR)}</select></label><label>Semester<select name="sem">${opts([1, 2, 3, 4, 5, 6, 7, 8])}</select></label></div>
    <div class="form-two"><label>Exam type${examSel()}</label><label>Academic year<input name="year" type="number" min="2020" max="2030" value="2026" required></label></div>
    <label>PDF file<input name="file" type="file" accept="application/pdf" required></label><button class="button button-dark form-submit">Upload paper</button></form>`,
  mine: () => {
    const p = editId && manager.findById(editId);
    return head("My papers", "Updating a paper sends it back for verification.") + pTbl(mine(), x => btn("edit", x.id, "Update") + btn("delreq", x.id, "Request deletion")) +
      (p ? `<form class="upload-form edit-box" data-form="edit"><h3>Update ${esc(p.name)}</h3><div class="form-two"><label>Exam type${examSel(p.type)}</label><label>Academic year<input name="year" type="number" min="2020" max="2030" value="${p.year}" required></label></div>
      <div class="modal-actions"><button class="button button-dark button-small">Save changes</button><button type="button" class="mini" data-act="cancel">Cancel</button></div></form>` : "");
  },
  reports: () => head("Reports") + stats([["Total papers", manager.papers.length], ["Verified", count("Verified")], ["Pending", count("Pending")], ["Rejected", count("Rejected")], ["Deletion requests", count("Deletion Requested")]]),
  verify: () => head("Verify papers", "Pending uploads and deletion requests.") + pTbl(manager.papers.filter(p => ["Pending", "Deletion Requested"].includes(p.status)), p => btn("verify", p.id, "Verify") + btn("reject", p.id, "Reject")),
  all: () => head("Manage papers") + `<div class="modal-actions">${btn("sort", "", "Sort by year")}</div><br>` + pTbl(manager.papers, p => btn("verify", p.id, "Verify") + btn("reject", p.id, "Reject") + btn("mark", p.id, "Mark for deletion")),
  users: () => head("Manage users") + grid(["ID", "Name", "Role", "Email"], Object.values(users).map(u => [u.id, esc(u.name), u.role, esc(u.email)]))
};
function renderPortal() {
  $("#portalKicker").textContent = `PAPERVAULT / ${me.role.toUpperCase()} DASHBOARD`;
  $("#portalNav").innerHTML = NAV[me.role].map(([k, l]) => `<button data-act="tab" data-id="${k}"${k === tab ? ' class="on"' : ""}>${l}</button>`).join("") + `<button class="out" data-act="logout">Log out</button>`;
  $("#portalMain").innerHTML = (V[tab] || V.home)();
}
function openPortal(t) {
  tab = t || NAV[me.role][0][0]; editId = null; $("#loginBtn").textContent = "Dashboard";
  renderPortal(); closeAll(); openM("#portalModal");
}

/* ---------- Events ---------- */
document.addEventListener("click", e => {
  if (e.target.matches(".modal-backdrop") || e.target.closest("[data-close]")) return closeAll();
  const r = e.target.closest("[data-role]"); if (r) { role = r.dataset.role; return syncRole(); }
  const b = e.target.closest("[data-act]"); if (!b) return;
  const id = b.dataset.id, p = manager.findById(id), isAdmin = me instanceof Admin, own = me instanceof Faculty && p && p.by === me.id;
  const setS = (s, m) => { if (!isAdmin || !p) return; p.setStatus(s); persist(); renderPortal(); toast(m); };
  const keep = (list, m) => { if (!(me instanceof Student) || !p) return; if (!me[list].includes(p.id)) me[list].push(p.id); persist(); toast(m); };
  ({
    view: () => showPaper(id), login: () => openLogin("Student"),
    tab: () => { if (id === "browse") { closeAll(); return $("#browse").scrollIntoView(); } tab = id; editId = null; renderPortal(); },
    logout: () => { me = null; closeAll(); $("#loginBtn").textContent = "Log in"; toast("Logged out successfully."); },
    download: () => keep("downloads", "Download recorded in My downloads."), save: () => keep("saved", "Paper saved."),
    verify: () => setS("Verified", "Paper verified."), reject: () => setS("Rejected", "Paper rejected."), mark: () => setS("Deletion Requested", "Paper marked for deletion."),
    sort: () => { if (isAdmin) { manager.sortByYear(); persist(); renderPortal(); toast("Papers sorted by academic year."); } },
    edit: () => { if (own) { editId = p.id; renderPortal(); } }, cancel: () => { editId = null; renderPortal(); },
    delreq: () => { if (own) { p.setStatus("Deletion Requested"); persist(); renderPortal(); toast("Deletion request sent to Admin."); } else toast("Invalid paper selection."); }
  })[b.dataset.act]?.();
});
document.addEventListener("submit", e => {
  const f = e.target; e.preventDefault();
  if (f.id === "loginForm") {
    if (users[role].login($("#loginPw").value)) { me = users[role]; openPortal(); toast(`Login successful. Logged in as ${me.role}.`); }
    else { $("#loginMsg").textContent = "Incorrect password."; $("#loginMsg").classList.add("err"); }
  } else if (f.dataset.form === "upload" && me instanceof Faculty) {
    const d = new FormData(f), file = d.get("file");
    if (!file || !/\.pdf$/i.test(file.name)) return toast("Choose a PDF file.");
    manager.addPaper(new ExamPaper(manager.generateId(), d.get("code").trim().toUpperCase(), d.get("name").trim(), d.get("branch"), +d.get("sem"), d.get("type"), +d.get("year"), file.name, me.id));
    persist(); tab = "mine"; renderPortal(); toast("Paper uploaded. Status: Pending admin verification.");
  } else if (f.dataset.form === "edit") {
    const p = manager.findById(editId), d = new FormData(f);
    if (!p || p.by !== me.id) return;
    p.setExamDetails(d.get("type"), +d.get("year")); p.setStatus("Pending"); editId = null; persist(); renderPortal();
    toast("Paper updated. Status changed to Pending for verification.");
  } else if (f.id === "heroSearch") {
    $("#filterQuery").value = $("#heroQuery").value; limit = 6; renderGrid(); $("#browse").scrollIntoView();
  }
});
document.addEventListener("keydown", e => { if (e.key === "Escape") closeAll(); });
$("#loginBtn").onclick = () => me ? openPortal() : openLogin();
$("#openUpload").onclick = $("#contributeTop").onclick = contribute;
$("#mobileMenu").onclick = () => $("#mainNav").classList.toggle("open");
$("#mainNav").onclick = () => $("#mainNav").classList.remove("open");
const reset = () => { ["#filterQuery", "#examFilter", "#branchFilter", "#semesterFilter"].forEach(s => $(s).value = ""); $("#sortFilter").value = "recent"; limit = 6; renderGrid(); };
$("#resetFilters").onclick = $("#emptyReset").onclick = reset;
$("#loadMore").onclick = () => { limit += 6; renderGrid(); };
["#filterQuery", "#examFilter", "#branchFilter", "#semesterFilter", "#sortFilter"].forEach(s => $(s).addEventListener("input", () => { limit = 6; renderGrid(); }));
renderGrid();
