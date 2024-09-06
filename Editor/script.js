// Get DOM elements
const codeTextarea = document.getElementById('code-textarea');
const compileButton = document.getElementById('compile-button');
const outputDisplay = document.getElementById('output-display');
const errorContainer = document.getElementById('error-container');
const tabButtons = document.querySelectorAll('.tab-button');
const fileInput = document.getElementById('file-input');
const themeToggle = document.getElementById('theme-toggle');
const moonIcon = themeToggle.querySelector('.moon');
const sunIcon = themeToggle.querySelector('.sun');

// Initialize state
let currentTab = 'lexer';
let output = {
    lexer: '',
    trees: '',
    semantic: '',
    codegen: ''
};

// Event listeners
compileButton.addEventListener('click', handleCompile);
tabButtons.forEach(button => {
    button.addEventListener('click', () => switchTab(button.dataset.tab));
});
fileInput.addEventListener('change', handleFileUpload);
themeToggle.addEventListener('click', toggleTheme);

function handleCompile() {
    const code = codeTextarea.value;
    errorContainer.innerHTML = '';

    // Simulated compilation process
    output = {
        lexer: `Lexer output for:\n${code}`,
        trees: 'Parse tree output...',
        semantic: 'Semantic analysis output...',
        codegen: 'Generated code output...'
    };

    updateOutput();
}

function switchTab(tab) {
    currentTab = tab;
    tabButtons.forEach(button => {
        button.classList.toggle('active', button.dataset.tab === tab);
    });
    updateOutput();
}

function updateOutput() {
    outputDisplay.textContent = output[currentTab];
}

function showError(message) {
    errorContainer.innerHTML = `
        <div class="error-message">
            <strong>Error:</strong> ${message}
        </div>
    `;
}

function handleFileUpload(event) {
    const file = event.target.files[0];
    if (file) {
        const reader = new FileReader();
        reader.onload = function(e) {
            codeTextarea.value = e.target.result;
        };
        reader.onerror = function(e) {
            showError('Error reading file');
        };
        reader.readAsText(file);
    }
}

function toggleTheme() {
    document.documentElement.classList.toggle('dark');
    updateThemeIcon();
}

function updateThemeIcon() {
    const isDark = document.documentElement.classList.contains('dark');
    moonIcon.style.display = isDark ? 'none' : 'block';
    sunIcon.style.display = isDark ? 'block' : 'none';
}

// Initial theme icon update
updateThemeIcon();
