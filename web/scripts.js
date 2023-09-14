function openFile() {
  $("#open_file").blur();

  function readFile(e) {
    var file = e.target.files[0];
    if (!file) return;

    var reader = new FileReader();
    reader.onload = function (e) {
      var contents = e.target.result;
      document.getElementById("codeArea").value = contents;
      updateLineNumbers();
      document.body.removeChild(fileInput);
    };
    reader.readAsText(file);
    open_file_name = file.name;
  }

  var fileInput = document.createElement("input");
  fileInput.type = "file";
  fileInput.accept = ".txt"; // Restrict to .txt files only
  fileInput.style.display = "none";
  fileInput.onchange = readFile;

  document.body.appendChild(fileInput);
  fileInput.click(); // Using the built-in click method to programmatically open the file picker
}
  
const codeArea = document.getElementById("codeArea");
const lineNumbers = document.getElementById("lineNumbers");
  
  // Update line numbers
function updateLineNumbers() {
  const lines = codeArea.value.split("\n").length;
  let lineStr = "";
  for (let i = 1; i <= lines; i++) {
    lineStr += i + "\n";
  }
  lineNumbers.textContent = lineStr;
}
  
codeArea.addEventListener("input", updateLineNumbers);
  
  // Synchronize scrolling
codeArea.addEventListener("scroll", function () {
  lineNumbers.scrollTop = codeArea.scrollTop;
});
  
document.getElementById("toggle-theme").addEventListener("click", function () {
  var bodyElem = document.body;
  var iconElem = document.querySelector("#toggle-theme i.fas");

  if (bodyElem.classList.contains("dark")) {
    bodyElem.classList.remove("dark");
    iconElem.classList.remove("fa-sun");
    iconElem.classList.add("fa-moon");
  } else {
    bodyElem.classList.add("dark");
    iconElem.classList.remove("fa-moon");
    iconElem.classList.add("fa-sun");
  }
});

let Modules = {};
  
Modules["onRuntimeInitialized"] = function () {
  // WASM is ready
  document.getElementById("run-btn").disabled = false; // enable the Compile button
};
  
function run_cmake() {
  const codeInput = document.getElementById("codeArea");
  const content = codeInput.value;
  
  if (content.trim() !== "") {
    try {
      const result = Module.processFileContent(content);
      displayResults(result);
    } catch (wasmError) {
      console.error("Error from WASM", wasmError);
  
      // Hide all tabs
      document
        .querySelectorAll(".nav-tabs .nav-item")
        .forEach((tab) => (tab.style.display = "none"));
  
      // Show error tab and its content
      const errorTab = document
        .getElementById("error-tab")
        .closest(".nav-item");
      errorTab.style.display = "block";
  
      $("#error-tab").tab("show"); // <-- Use jQuery-based approach here
  
      // Populate the error message
      document.getElementById("error").textContent = wasmError.toString();
      }
  } else {
    alert("Please type some code or upload a file.");
  }
}

function displayResults(result) {
  document
    .querySelectorAll(".nav-tabs .nav-item")
    .forEach((tab) => (tab.style.display = "block"));

  // Hide the error tab
  document.getElementById("error-tab").closest(".nav-item").style.display =
    "none";

  const resultJSON = JSON.parse(result);
  let lexerContent = "";
  resultJSON.lexer.forEach((item) => {
    lexerContent += `[${item.value}]\t    ->   ${item.location};\t ${item.type}\n`;
  });

  document.getElementById("lexer").textContent = lexerContent;

  document.getElementById("symbol").innerHTML = generateTableFromJSON(
    resultJSON.semantic,
  );

  let fullTreeString = "";
  resultJSON.syntax.forEach((tree) => {
    const dimensions = getDimensions(tree);
    const grid = Array.from(
      {
        length: dimensions.height * 2 - 1,
      },
      () =>
        Array.from(
          {
            length: dimensions.width,
          },
          () => " ",
        ),
    );

    buildTree(tree, grid);
    fullTreeString += grid.map((row) => row.join("")).join("\n") + "\n\n";
  });

  // Update the content of the <pre> element
  document.getElementById("parser").textContent = fullTreeString.trim();

  // Update the content of the <pre> element

  const assemblyCode = transformJSONToAssembly(resultJSON.codegen);
  document.getElementById("codegen").textContent = assemblyCode;

  $("#lexer-tab").tab("show");
}
  
function getDimensions(node) {
  if (!node) {
    return {
    height: 0,
    width: 0,
    };
  }
  const leftDims = getDimensions(node.left);
  const rightDims = getDimensions(node.right);
  return {
    height: 1 + Math.max(leftDims.height, rightDims.height),
    width: 1 + leftDims.width + rightDims.width,
  };
}
  
function buildTree(node, grid, row = 0, col = 0) {
  if (!node) return;

  const leftDims = getDimensions(node.left);
  const rightDims = getDimensions(node.right);

  grid[row][col + leftDims.width] = node.type.value;

  if (node.left) {
    grid[row + 1][col + leftDims.width - 1] = "/";
    buildTree(node.left, grid, row + 2, col);
  }

  if (node.right) {
    grid[row + 1][col + leftDims.width + 1] = "\\";
    buildTree(node.right, grid, row + 2, col + leftDims.width + 1);
  }
}

function generateTableFromJSON(data) {
  // Create the table headers
  let tableString = '<table border="1">\n';
  tableString += "  <thead>\n";
  tableString += "    <tr>\n";
  tableString += "      <th>Type</th>\n";
  tableString += "      <th>Variable</th>\n";
  tableString += "    </tr>\n";
  tableString += "  </thead>\n";
  tableString += "  <tbody>\n";

  // Fill the table rows with data
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
  let assemblyCode = "";

  commandsArray.forEach((commandGroup) => {
    commandGroup.forEach((command) => {
      if (command) {
        let line = "\t";
        for (let key in command) {
          if (Array.isArray(command[key])) {
            line += command[key].join(", ");
          } else {
            line += command[key] + " ";
          }
        }
        assemblyCode += line.trim() + "\n";
      }
    });
  });
  assemblyCode += "\t bx lr\n\n";

  return assemblyCode;
}
