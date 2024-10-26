// Monaco Editor Initialization
let editor;

require.config({ paths: { vs: 'https://cdnjs.cloudflare.com/ajax/libs/monaco-editor/0.36.1/min/vs' } });
require(["vs/editor/editor.main"], function () {
  // custom dark theme
  monaco.editor.defineTheme("custom-dark", {
    base: "vs-dark",
    inherit: true,
    rules: [],
    colors: {
      "editor.background": "#292c35"
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
themeSelect.value = document.body.classList.contains('dark') ? "Dark" : "Light";
themeSelect.addEventListener("change", () => {
  document.body.classList.toggle("dark", themeSelect.value === "Dark");
  monaco.editor.setTheme(themeSelect.value === "Dark" ? "custom-dark" : "vs-light");
});


// dummy code for the editor
document.addEventListener("DOMContentLoaded", () => {
  const insertCodeButton = document.getElementById("insert-code-btn");
  insertCodeButton.addEventListener("click", () => {
    const dummyCode = `x = 5\ny = 10\nprint(x + y)`;
    editor.setValue(dummyCode);
  });
});

// Clear button functionality
document.querySelector(".clear-btn").addEventListener("click", () => {
  if (editor) editor.setValue(""); // Clear editor
  document.querySelectorAll(".output-pane").forEach(pane => (pane.innerHTML = ""));
});


// compile button click
document.getElementById("run-btn").addEventListener("click", handleCompile);

function handleCompile() {
  const code = editor.getValue();
  const errorContainer = document.getElementById("error-container");
  hideError();

  if (code.trim() === "") {
    showError("Please type some code or upload a file.");
    return;
  }

  try {
    const result = Module.processFileContent(code);
    const parsedResult = JSON.parse(result);

    if (!parsedResult.success) {
      throw new Error(parsedResult.error);
    }

    displayResults(parsedResult);
  } catch (error) {
    console.error("Error:", error);
    showError(error.message || error.toString());
  }
}

// Display compilation results in the output panes
function clearResults() {
  document.getElementById('lexer-output').innerHTML =  "probably an error occurred ..";
  document.getElementById('parse-tree-output').innerHTML = "probably an error occurred ..";
  document.getElementById('symbol-table-output').innerHTML = "probably an error occurred ..";
  document.getElementById('code-gen-output').innerHTML = "probably an error occurred ..";
}

// Display compilation results in the output panes
function displayResults(parsedResult) {
  document.getElementById('lexer-output').innerHTML = formatLexerOutput(parsedResult.Lexer);
  document.getElementById('parse-tree-output').innerHTML = generateAsciiTree(parsedResult.AST);
  document.getElementById('symbol-table-output').innerHTML = generateTable(parsedResult.SymbolTable);
  document.getElementById('code-gen-output').innerHTML = generateGenOutput(parsedResult.Gen, parsedResult.Local);
}

function showError(error) {
  clearResults();
  const errorContainer = document.getElementById('error-container');
  const errorMessage = document.getElementById('error-message');
  errorMessage.textContent = error;
  errorContainer.classList.remove('hidden');
}

function hideError() {
  const errorContainer = document.getElementById('error-container');
  errorContainer.classList.add('hidden');
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

    btn.classList.add(colorClasses[index], "font-bold", "text-red-900");
    btn.classList.remove("text-gray-600");
    document.getElementById(btn.dataset.target).classList.remove("hidden");
  });
});



function formatLexerOutput(lexerData) {

  const formattedTokens = lexerData.map(token => {
    const position = token.location.replace(/[<>]/g, '').padEnd(25);
    const tokenType = token.type.padEnd(20);
    const value = formatValue(token.value);

    return `${tokenType}${position}${[value]}`;
  });

  return formattedTokens.join('\n');
}

// Helper function to format special characters in the value
function formatValue(value) {
  switch (value) {
    case "\\n":
      return "[\\n]";
    case "\\t":
      return "[\\t]";
    case "":
      return "[<empty>]";
    default:
      return `[${value}]`; // Wrap regular values in brackets
  }
}

function generateAsciiTree(node, prefix = "", isLast = true, depth = 0) {
  if (!node) return "";

  let result = "";
  const indent = prefix + (isLast ? "└─ " : "├─ ");

  // Add token value or indicate it's unknown
  if (node.token && node.token.value) {
    result += indent + node.token.value + "\n";
  } else {
    result += indent + "Unknown\n";
  }

  // Handle different node types
  if (Array.isArray(node.children) && node.children.length > 0) {
    node.children.forEach((child, index) => {
      const isLastChild = index === node.children.length - 1;
      result += generateAsciiTree(child, prefix + (isLast ? "    " : "│   "), isLastChild, depth + 1);
    });
  } else if (node.left && node.right) {
    // Handle left-right child nodes
    result += generateAsciiTree(node.left, prefix + (isLast ? "    " : "│   "), false, depth + 1);
    result += generateAsciiTree(node.right, prefix + (isLast ? "    " : "│   "), true, depth + 1);
  } else if (node.condition && node.ifNode && node.elseNode) {
    // Handle conditionals
    result += generateAsciiTree(node.condition, prefix + (isLast ? "    " : "│   "), false, depth + 1);
    result += generateAsciiTree(node.ifNode, prefix + (isLast ? "    " : "│   "), false, depth + 1);
    result += generateAsciiTree(node.elseNode, prefix + (isLast ? "    " : "│   "), true, depth + 1);
  } else if (node.condition && node.ifNode) {
    // Handle conditionals without an elseNode (optional else)
    result += generateAsciiTree(node.condition, prefix + (isLast ? "    " : "│   "), false, depth + 1);
    result += generateAsciiTree(node.ifNode, prefix + (isLast ? "    " : "│   "), true, depth + 1);
  }

  return result;
}

function generateTable(symbolTable) {
  const formattedEntries = symbolTable.flatMap(entry =>
    Object.values(entry).map(symbol => {
      const typePadded = symbol.type.padEnd(15);
      const valuePadded = symbol.value.padEnd(10);
      return `${typePadded}${valuePadded}\n`;
    })
  );

  return formattedEntries.join("");
}

function generateGenOutput(genData, Local) {
  let output = "\n";

  // Generate WASM code instructions
  genData[0].forEach((instruction) => {
    output += `${instruction}\n`;
  });

  // Add a separator between the codegen output and local variables
  output += "\n\n\nLocal Variables:\n";
  output += "-----------------\n";
  output += "Index       Variable\n";

  // Display the Local variables in a table-like format
  Local.forEach((local) => {
    const index = "local  " + local.index.toString().padEnd(8);
    const name = local.name;
    output += `${index}${name}\n`;
  });

  return output;
}



