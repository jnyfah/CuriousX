// Monaco Editor Initialization
require.config({ paths: { vs: 'https://cdnjs.cloudflare.com/ajax/libs/monaco-editor/0.36.1/min/vs' }});
require(["vs/editor/editor.main"], function() {
  monaco.editor.create(document.getElementById("editor-container"), {
    value: "// Write your code here...\n",
    language: "python",
    theme: "vs-light",
    automaticLayout: true,
  });
});


// Tab Switching with Bold and Color
const tabButtons = document.querySelectorAll(".tab-btn");
const outputPanes = document.querySelectorAll(".output-pane");

// Color classes for active tabs
const colorClasses = ["border-blue-600", "border-green-600", "border-red-600", "border-yellow-600"];

tabButtons.forEach((btn, index) => {
  btn.addEventListener("click", () => {
    // Reset all tabs and panes
    tabButtons.forEach((b) => {
      b.classList.remove(...colorClasses, "font-bold", "text-red-900");
      b.classList.add("text-gray-600");
    });
    outputPanes.forEach((pane) => pane.classList.add("hidden"));

    // Activate the selected tab and show the corresponding pane
    btn.classList.add(colorClasses[index], "font-bold", "text-red-900");
    btn.classList.remove("text-gray-600");
    document.getElementById(btn.dataset.target).classList.remove("hidden");
  });
});

// Run Code Button Logic
document.getElementById("run-btn").addEventListener("click", () => {
  document.getElementById("lexer-output").textContent = "Lexer output...";
  document.getElementById("parse-tree-output").textContent = "Parse tree...";
  document.getElementById("symbol-table-output").textContent = "Symbol table...";
  document.getElementById("code-gen-output").textContent = "Generated code...";
});

// Clear Output Button Logic
document.getElementById("clear-output").addEventListener("click", () => {
  outputPanes.forEach((pane) => (pane.textContent = ""));
});

// Hide Settings Button Logic
document.getElementById("hide-settings").addEventListener("click", () => {
  const sidebar = document.getElementById("sidebar");
  sidebar.classList.toggle("hidden");
});
