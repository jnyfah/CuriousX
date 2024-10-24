// Monaco Editor Initialization
let editor;

require.config({ paths: { vs: 'https://cdnjs.cloudflare.com/ajax/libs/monaco-editor/0.36.1/min/vs' } });
require(["vs/editor/editor.main"], function () {
  // Define custom dark theme
  monaco.editor.defineTheme('custom-dark', {
    base: 'vs-dark',
    inherit: true,
    rules: [],
    colors: {
      'editor.background': '#292c35'
    }
  });

  editor = monaco.editor.create(document.getElementById("editor-container"), {
    value: "// Write your code here...\n",
    language: "python",
    theme: document.body.classList.contains('dark') ? 'custom-dark' : 'vs-light',
    automaticLayout: true,
  });
});

const themeSelect = document.getElementById("theme-select");

// Set initial select value based on body class
themeSelect.value = document.body.classList.contains('dark') ? 'Dark' : 'Light';
themeSelect.addEventListener("change", () => {
  document.body.classList.toggle("dark", themeSelect.value === "Dark");
  monaco.editor.setTheme(themeSelect.value === "Dark" ? 'custom-dark' : 'vs-light');
});


document.addEventListener('DOMContentLoaded', () => {
  const insertCodeButton = document.getElementById('insert-code-btn'); // Select the right button

  // Handle button click to insert code into Monaco editor
  insertCodeButton.addEventListener('click', () => {
    const dummyCode = `x = 5\ny = 10\nprint(x + y)`; // Dummy code

    // Insert the dummy code into the editor
    editor.setValue(dummyCode);
  });
});


// Handle the compile button click
document.getElementById('run-btn').addEventListener('click', handleCompile);

// Clear button functionality
document.querySelector('.clear-btn').addEventListener('click', () => {
  if (editor) editor.setValue(''); // Clear editor
  document.querySelectorAll('.output-pane').forEach(pane => (pane.innerHTML = '')); // Clear output panes
});


function handleCompile() {
  const code = editor.getValue(); // Get code from Monaco editor
  const errorContainer = document.getElementById('error-container');
  errorContainer.innerHTML = ""; // Clear previous errors

  if (code.trim() === "") {
      showError("Please type some code or upload a file.");
      return;
  }

  try {
      const result = Module.processFileContent(code); // Process the code (assuming `Module` exists)
      const parsedResult = JSON.parse(result);

      if (!parsedResult.success) {
          throw new Error(parsedResult.error); // Throw an error if compilation fails
      }

      displayResults(parsedResult); // Display results on success
  } catch (error) {
      console.error("Error:", error);
      showError(error.message || error.toString());
  }
}


// Display compilation results in the output panes
function displayResults(parsedResult) {
  document.getElementById('lexer-output').innerHTML = parsedResult.Lexer || "No lexer output";
  document.getElementById('parse-tree-output').innerHTML = parsedResult.AST || "No parse tree output";
  document.getElementById('symbol-table-output').innerHTML = parsedResult.symbolTable || "No symbol table output";
  document.getElementById('code-gen-output').innerHTML = parsedResult.generatedCode || "No generated code";
}

// Show error messages
function showError(message) {
  const errorContainer = document.getElementById('error-container');
  errorContainer.innerHTML = `<p class="text-red-500">${message}</p>`;
}


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



// Clear Output Button Logic
document.getElementById("clear-output").addEventListener("click", () => {
  outputPanes.forEach((pane) => (pane.textContent = ""));
});

// Hide Settings Button Logic
document.getElementById("hide-settings").addEventListener("click", () => {
  const sidebar = document.getElementById("sidebar");
  sidebar.classList.toggle("hidden");
});


