using System;
using System.Drawing;
using System.IO;
using System.Security.Cryptography;
using System.Text;
using System.Threading.Tasks;
using System.Diagnostics;
using System.Windows.Forms;

namespace MxCADTest
{
    public partial class Form1 : Form
    {
        private MxCADAppWrapper _app;
        private MxCADViewWrapper _view;

        // Dedicated panel container for displaying CAD content
        private Panel _renderPanel;

        public Form1()
        {
            InitializeComponent();
            this.Text = "MxCAD C# Test Window";
            this.Size = new System.Drawing.Size(1024, 768);

            // 1. Initialize menu (added first for layout order)
            InitializeCustomMenu();

            // 2. Initialize render panel (added after menu)
            InitializeRenderPanel();

            this.Load += Form1_Load;
            this.FormClosed += Form1_FormClosed;
        }

        private void InitializeCustomMenu()
        {
            MenuStrip menuStrip = new MenuStrip();
            ToolStripMenuItem fileMenu = new ToolStripMenuItem("File(&F)");
            ToolStripMenuItem openItem = new ToolStripMenuItem("Open File(&O)...");

            openItem.Click += OpenItem_Click;
            openItem.ShortcutKeys = Keys.Control | Keys.O;

            fileMenu.DropDownItems.Add(openItem);
            menuStrip.Items.Add(fileMenu);

            this.Controls.Add(menuStrip);
            this.MainMenuStrip = menuStrip;
        }

        // Initialize display panel
        private void InitializeRenderPanel()
        {
            _renderPanel = new Panel();

            _renderPanel.Dock = DockStyle.Fill;

            _renderPanel.BackColor = Color.Black;

            _renderPanel.Resize += _renderPanel_Resize;

            // Add to form controls
            this.Controls.Add(_renderPanel);

            // Ensure panel is behind menu strip (Z-Order) to prevent blocking dropdown menu
            _renderPanel.BringToFront();
        }

        private void Form1_Load(object sender, EventArgs e)
        {
            try
            {
                _app = new MxCADAppWrapper();
                Console.WriteLine("MxCAD App initialized successfully");

                _view = new MxCADViewWrapper();

                // 1. Pass _renderPanel.Handle (panel handle)
                // 2. Pass coordinates 0, 0 (relative to panel top-left)
                // 3. Use current panel width and height for dimensions
                bool ret = _view.Create(_renderPanel.Width, _renderPanel.Height, _renderPanel.Handle, true, 0, 0);

                if (ret)
                {
                    // MessageBox.Show("MxCAD view created successfully!");
                    _view.UpdateDisplay();
                }
                else
                {
                    MessageBox.Show("Failed to create MxCAD view!");
                }
            }
            catch (TypeInitializationException typeEx)
            {
                Exception inner = typeEx;
                while (inner.InnerException != null) inner = inner.InnerException;
                MessageBox.Show($"【Initialization Failed】\nException Type: {inner.GetType().Name}\nError Message: {inner.Message}");
            }
        }

        // Logic for panel resize event
        private void _renderPanel_Resize(object sender, EventArgs e)
        {
            if (_view != null && _renderPanel.Handle != IntPtr.Zero)
            {
                _view.SetMxDrawPosition(0, 0, _renderPanel.Width, _renderPanel.Height);
            }
        }

        private async void OpenItem_Click(object sender, EventArgs e)
        {
            OpenFileDialog openFileDialog = new OpenFileDialog();
            openFileDialog.Filter = "CAD Files (*.mxweb;*.dwg;*.dxf)|*.mxweb;*.dwg;*.dxf|All files (*.*)|*.*";

            if (openFileDialog.ShowDialog() == DialogResult.OK)
            {
                string filePath = openFileDialog.FileName;
                await OpenFileLogic(filePath);
            }
        }

        private async Task OpenFileLogic(string filePath)
        {
            if (_view == null) return;
            string suffix = Path.GetExtension(filePath).ToLower().TrimStart('.');

            if (suffix == "mxweb")
            {
                OpenMxWebFile(filePath);
            }
            else if (suffix == "dwg" || suffix == "dxf")
            {
                await ConvertAndOpenFile(filePath);
            }
            else
            {
                MessageBox.Show("Unsupported file format");
            }
        }

        private async Task ConvertAndOpenFile(string filePath)
        {
            try
            {
                long timestamp = new FileInfo(filePath).LastWriteTime.Ticks;
                string modifiedStr = timestamp.ToString();
                string outNameHash = CalculateMD5(filePath + modifiedStr);

                string docPath = Environment.GetFolderPath(Environment.SpecialFolder.MyDocuments);
                string subDir = Path.Combine(docPath, "MxCADViewer");
                if (!Directory.Exists(subDir)) Directory.CreateDirectory(subDir);

                string outFileName = outNameHash + ".mxweb";
                string outFilePath = Path.Combine(subDir, outFileName);

                if (File.Exists(outFilePath))
                {
                    OpenMxWebFile(outFilePath);
                    return;
                }

                string appPath = AppDomain.CurrentDomain.BaseDirectory;
                string mxConvertPath = Path.Combine(appPath, "mxconvert.exe");

                if (!File.Exists(mxConvertPath))
                {
                    MessageBox.Show("mxconvert.exe not found: " + mxConvertPath);
                    return;
                }

                ProcessStartInfo psi = new ProcessStartInfo();
                psi.FileName = mxConvertPath;
                psi.Arguments = $"-s \"{filePath}\" -o \"{outFilePath}\"";
                psi.UseShellExecute = false;
                psi.CreateNoWindow = true;
                psi.WorkingDirectory = Path.GetDirectoryName(mxConvertPath);

                this.Text = "Converting...";
                this.Cursor = Cursors.WaitCursor;

                bool convertSuccess = await RunProcessAsync(psi, 600000);

                this.Cursor = Cursors.Default;
                this.Text = "MxCAD C# Test Window";

                if (convertSuccess && File.Exists(outFilePath))
                {
                    OpenMxWebFile(outFilePath);
                }
                else
                {
                    MessageBox.Show("Conversion failed");
                }
            }
            catch (Exception ex)
            {
                this.Cursor = Cursors.Default;
                MessageBox.Show("Error: " + ex.Message);
            }
        }

        private void OpenMxWebFile(string fullPath)
        {
            if (_view.ReadFile(fullPath))
            {
                _view.UpdateDisplay();
            }
            else
            {
                MessageBox.Show("Failed to open: " + fullPath);
            }
        }

        private string CalculateMD5(string input)
        {
            using (MD5 md5 = MD5.Create())
            {
                byte[] inputBytes = Encoding.UTF8.GetBytes(input);
                byte[] hashBytes = md5.ComputeHash(inputBytes);
                StringBuilder sb = new StringBuilder();
                for (int i = 0; i < hashBytes.Length; i++) sb.Append(hashBytes[i].ToString("x2"));
                return sb.ToString();
            }
        }

        private Task<bool> RunProcessAsync(ProcessStartInfo psi, int timeoutMs)
        {
            var tcs = new TaskCompletionSource<bool>();
            Process process = new Process();
            process.StartInfo = psi;
            process.EnableRaisingEvents = true;
            process.Exited += (s, e) => { tcs.TrySetResult(process.ExitCode == 0); process.Dispose(); };

            try { process.Start(); }
            catch { return Task.FromResult(false); }

            var timeoutTask = Task.Delay(timeoutMs);
            return Task.WhenAny(tcs.Task, timeoutTask).ContinueWith(t =>
            {
                if (t.Result == timeoutTask) { try { process.Kill(); } catch { } return false; }
                return tcs.Task.Result;
            });
        }

        private void Form1_FormClosed(object sender, FormClosedEventArgs e)
        {
            if (_view != null)
            {
                _view.Free();
                _view.Dispose();
            }
            if (_app != null)
            {
                _app.Free();
                _app.Dispose();
            }
        }
    }
}