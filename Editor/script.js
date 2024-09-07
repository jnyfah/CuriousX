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
  lexer: "",
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
      displayResults(JSON.parse(result));
    } catch (wasmError) {
      console.error("Error from WASM", wasmError);
      showError(wasmError.toString());
    }
  } else {
    showError("Please type some code or upload a file.");
  }
}

function displayResults(result) {
  output.lexer = formatLexerOutput(result.lexer);
  output.trees = formatTreeOutput(result.syntax);
  output.semantic = generateTableFromJSON(result.semantic);
  output.codegen = transformJSONToAssembly(result.codegen);

  updateOutput();
  showAllTabs();
}

function formatLexerOutput(lexerData) {
  return lexerData
    .map((item) => `[${item.value}] -> ${item.location}; ${item.type}`)
    .join("\n");
}

function formatTreeOutput(syntaxTrees) {
  return syntaxTrees
    .map((tree) => {
      const dimensions = getDimensions(tree);
      const grid = Array.from({ length: dimensions.height * 2 - 1 }, () =>
        Array(dimensions.width).fill(" ")
      );
      buildTree(tree, grid);
      return grid.map((row) => row.join("")).join("\n");
    })
    .join("\n\n");
}

function getDimensions(node) {
  if (!node) return { height: 0, width: 0 };
  const left = getDimensions(node.left);
  const right = getDimensions(node.right);
  return {
    height: 1 + Math.max(left.height, right.height),
    width: 1 + left.width + right.width,
  };
}

function buildTree(node, grid, row = 0, col = 0) {
  if (!node) return;
  const leftDims = getDimensions(node.left);
  const midCol = col + leftDims.width;

  grid[row][midCol] = node.type.value;

  if (node.left) {
    grid[row + 1][midCol - 1] = "/";
    buildTree(node.left, grid, row + 2, col);
  }

  if (node.right) {
    grid[row + 1][midCol + 1] = "\\";
    buildTree(node.right, grid, row + 2, midCol + 1);
  }
}

function generateTableFromJSON(data) {
  let tableString = "<table border='1'>\n";
  tableString += "  <thead>\n";
  tableString += "    <tr>\n";
  tableString += "      <th>Type</th>\n";
  tableString += "      <th>Variable</th>\n";
  tableString += "    </tr>\n";
  tableString += "  </thead>\n";
  tableString += "  <tbody>\n";

  data.forEach((item) => {
    tableString += "    <tr>\n";
    tableString += `      <td>${item.type}</td>\n`;
    tableString += `      <td>${item.value}</td>\n`;
    tableString += "    </tr>\n";
  });

  tableString += "  </tbody>\n";
  tableString += "</table>\n";

  return tableString;
}

function transformJSONToAssembly(commandsArray) {
  const assemblyLines = commandsArray.flatMap((commandGroup) =>
    commandGroup
      .map((command) => {
        if (!command) return "";
        return (
          "\t" +
          Object.values(command)
            .map((val) => (Array.isArray(val) ? val.join(", ") : val))
            .join(" ")
            .trim()
        );
      })
      .filter(Boolean),
  );

  return assemblyLines.join("\n") + "\n\tbx lr\n";
}

function switchTab(tab) {
  currentTab = tab;
  tabButtons.forEach((button) => {
    button.classList.toggle("active", button.dataset.tab === tab);
  });
  updateOutput();
}

function updateOutput() {
  outputDisplay.textContent = output[currentTab];
}

function showAllTabs() {
  tabButtons.forEach((tab) => (tab.style.display = "block"));
}

function showError(message) {
  errorContainer.innerHTML = `<div class="error-message"><strong>Error:</strong> ${message}</div>`;
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
