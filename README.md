<div align="center">

  # GDH: Now for 2.2!
  ### GDH - Free, integrated and first Geometry Dash mod menu for 2.2!
  
  <a href="https://github.com/TobyAdd/GDH/releases" target="_blank" rel="noreferrer"> <img src="https://github.com/TobyAdd/GDH/assets/66429886/14aba1a5-4c0d-42fd-87da-e4af25b08dde" alt="GDH Menu" title="GDH Menu"/> </a>
</div>

---

<h2 align="center">How to use?</h2>

1. Open [latest release](https://github.com/TobyAdd/GDH/releases/latest) link
2. Download "**Release.zip**" archive
3. Extract all files from archive to **Geometry Dash** folder (that contains the **GeometryDash.exe**)
4. Run Geometry Dash and press: **Tab** for show integrated menu

---

<h2 align="center">Development</h2>

<p> Some dependencies are needed: <a href="https://git-scm.com/">Git</a> ; <a href="https://code.visualstudio.com/">Visual Studio Code</a> ; <a href="https://cmake.org/">CMake</a> </p>

Now you can directly clone this repo or you can fork it.

Run this in your terminal on the folder where you want all the code:

1. **Clone**: `git clone https://github.com/TobyAdd/GDH --recursive` & `cd GDH`

3. **Configure** `cmake -B build -A win32`

5. **Build:** `cmake --build build --config Release -j 4`

- **You should find the result** in `build/Release`

---

<p align="center"> Special Thanks: Ocornut for <a href="https://github.com/ocornut/imgui">Imgui</a> ; <a href="https://github.com/howhathe">howhathe</a> for the ReadMe </p>
