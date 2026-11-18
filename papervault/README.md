# PaperVault — Exam Paper Repository (website)

A working website for the C++ project in `endsem_terminal.cpp`. The C++ program is a console app and cannot run inside a browser, so `script.js` is a port of its logic: the same classes (`ExamPaper`, `User` > `Student` / `Faculty` / `Admin`, `PaperManager`), the same four sample papers, the same login IDs and the same Pending -> Verified / Rejected workflow.

## Run it
Open `index.html` in a modern browser. No install or build step. Everything is saved in the browser (localStorage).

## Demo accounts (password `1234` for all)
| Role | User ID | What they can do |
|---|---|---|
| Student | S1023 | search, view, download and save papers, browse subjects, see downloads and saved lists |
| Faculty | F014 | upload papers (start as Pending), update a paper (returns to Pending), request deletion |
| Admin | A002 | verify or reject papers, mark for deletion, sort by year, reports, users, subjects |

Visitors who are not logged in only see Verified papers.

## Limits
- Passwords and data live in the browser, so this is a demo, not a secure system. For real use, move the logic to a server and database.
- Uploads record the PDF file name only. No file is stored, and downloads are recorded without serving a PDF.
- The C++ "OOP Concepts Demonstration" menu is not part of the website.
