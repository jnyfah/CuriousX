// Get DOM elements
const codeTextarea = document.getElementById("code-textarea");
const compileButton = document.getElementById("compile-button");
const outputDisplay = document.getElementById("output-display");
const errorContainer = document.getElementById("error-container");
const tabButtons = document.querySelectorAll(".tab-button");
const fileInput = document.getElementById("file-input");
const themeToggle = document.getElementById("theme-toggle");
const moonIcon = themeToggle.querySelector(".moon");
const sunIcon = themeToggle.querySelector(".sun");

// Initialize state
let currentTab = "lexer";
let output = {
  Lexer: "",
  trees: "",
  semantic: "",
  codegen: "",
};

// Event listeners
compileButton.addEventListener("click", handleCompile);
tabButtons.forEach((button) => {
  button.addEventListener("click", () => switchTab(button.dataset.tab));
});
fileInput.addEventListener("change", handleFileUpload);
themeToggle.addEventListener("click", toggleTheme);

let Modules = {};
Modules["onRuntimeInitialized"] = function () {
  document.getElementById("run-btn").disabled = false; 
};

function handleCompile() {
  const code = codeTextarea.value;
  errorContainer.innerHTML = "";
  if (code.trim() !== "") {
    try {
      const result = Module.processFileContent(code);
      const parsedResult = JSON.parse(result);
      if (parsedResult.success === false) {
        // If the operation wasn't successful, throw the error
        throw new Error(parsedResult.error);
      }
      displayResults(parsedResult);
    } catch (error) {
      console.error("Error:", error);
      showError(error.message || error.toString());
    }
  } else {
    showError("Please type some code or upload a file.");
  }
}

function displayResults(result) {
  output.lexer = formatLexerOutput(result.Lexer);
  output.trees = generateAsciiTree(result.AST);
  //output.semantic = generateTable(result.SymbolTable);
  output.codegen = formatCodegenOutput(result.Gen);

  updateOutput();
  showAllTabs();
}

function formatLexerOutput(lexerData) {
  const header =
    "Token            Position            Value\n" +
    "----------------------------------------------------------------------\n";

  const formattedTokens = lexerData.map((item) => {
    const tokenPadded = item.type.padEnd(20);
    const locationPadded = item.location.padEnd(20);
    const value = formatValue(item.value);

    return `${tokenPadded}${locationPadded}${value}\n`;
  });

  return header + formattedTokens.join("");
}

function formatValue(value) {
  if (value === "") return "<empty>";
  if (value === "\n") return "\\n";
  if (value === "\t") return "\\t";
  return `[${value}]`;
}


function switchTab(tab) {
  currentTab = tab;
  tabButtons.forEach((button) => {
    button.classList.toggle("active", button.dataset.tab === tab);
  });
  updateOutput();
}

function updateOutput() {
  outputDisplay.textContent = output[currentTab] || "";
}

function showAllTabs() {
  tabButtons.forEach((tab) => (tab.style.display = "block"));
}

function showError(message) {
  errorContainer.innerHTML = `<div class="error-message"><strong>Error:</strong> ${escapeHtml(message)}</div>`;
}

// Helper function to escape HTML special characters
function escapeHtml(unsafe) {
  return unsafe
    .replace(/&/g, "&amp;")
    .replace(/</g, "&lt;")
    .replace(/>/g, "&gt;")
    .replace(/"/g, "&quot;")
    .replace(/'/g, "&#039;");
}

function handleFileUpload(event) {
  const file = event.target.files[0];
  if (file) {
    const reader = new FileReader();
    reader.onload = function (e) {
      codeTextarea.value = e.target.result;
    };
    reader.onerror = function () {
      showError("Error reading file");
    };
    reader.readAsText(file);
  }
}

function toggleTheme() {
  document.documentElement.classList.toggle("dark");
  updateThemeIcon();
}

function updateThemeIcon() {
  const isDark = document.documentElement.classList.contains("dark");
  moonIcon.style.display = isDark ? "none" : "block";
  sunIcon.style.display = isDark ? "block" : "none";
}

// Initial theme icon update
updateThemeIcon();

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



function getNodeValue(node) {
  if (typeof node === "string") return node;
  if (node.token && node.token.value) return `${node.token.value}`;
  return "Unknown";
}

function getNodeChildren(node) {
  if (Array.isArray(node.children)) return node.children;
  if (node.left || node.right) return [node.left, node.right].filter(Boolean);
  return [];
}

function generateTable(symbolTable) {
  let tableHeader =
    "Type             Value\n" +
    "--------------------------------\n";

  // Format each entry in the symbol table
  const formattedEntries = symbolTable.map((entry) => {
    const typePadded = entry.type.padEnd(15); 
    const valuePadded = entry.value.padEnd(10); 
    return `${typePadded}${valuePadded}\n`;
  });

  return tableHeader + formattedEntries.join("");
}

function formatCodegenOutput(codegenData) {
    return codegenData.join('\n');
}