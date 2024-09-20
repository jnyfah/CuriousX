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
  // WASM is ready
  document.getElementById("run-btn").disabled = false; // enable the Compile button
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

  return header + formattedTokens.join('');
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
    outputDisplay.textContent = output[currentTab] || '';
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



function generateAsciiTree(node, prefix = "", isLast = true, isRoot = true) {
  if (!node) return "";
  let result = "";
 
  if (!isRoot) {
    result += prefix;
    result += isLast ? "└── " : "├── ";
  }
  // Get node value
  const nodeValue = getNodeValue(node);
  result += nodeValue + "\n";
  // Get children
  const children = getNodeChildren(node);
  // Generate subtrees
  children.forEach((child, index) => {
    const isLastChild = index === children.length - 1;
    const newPrefix = prefix + (isLast ? "    " : "│   ");
    result += generateAsciiTree(child, newPrefix, isLastChild, false);
  });
  return result;
}

function getNodeValue(node) {
  if (typeof node === 'string') return node;
  if (node.token && node.token.value) return `${node.token.value}`;
  return 'Unknown';
}

function getNodeChildren(node) {
  if (Array.isArray(node.children)) return node.children;
  if (node.left || node.right) return [node.left, node.right].filter(Boolean);
  return [];
}

