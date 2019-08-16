using System;
using System.IO;
using System.Windows.Forms;

namespace PakViewer
{
    public partial class Form1 : Form
    {
        private Pak pak;
        public Form1()
        {
            InitializeComponent();
        }

        private void ExitToolStripMenuItem_Click(object sender, EventArgs e)
        {
            Close();
        }

        private void OpenToolStripMenuItem_Click(object sender, EventArgs e)
        {
            DialogResult dialogResult = openFileDialog1.ShowDialog();
            if (dialogResult.Equals(DialogResult.Cancel))
            {
                return;
            }
            string filePath = openFileDialog1.FileName;
            string fileName = Path.GetFileName(filePath);

            bool doStrings;
            string stringsPath;

            dialogResult = MessageBox.Show("Do you want to load .strings file? Without .string filenames will be replaced by their hashes", "PAK Viewer", MessageBoxButtons.YesNo, MessageBoxIcon.Question);
            if (dialogResult.Equals(DialogResult.Yes))
            {
                doStrings = openFileDialog2.ShowDialog() != DialogResult.Cancel;
                stringsPath = openFileDialog2.FileName;
            } else
            {
                doStrings = false;
            }

            try
            {
                pak = new Pak(filePath);
                toolStripStatusLabel1.Text = $"{pak.mNumFiles} files in PAK";
                propertyGrid1.SelectedObject = pak;
            }
            catch (PakBadImageException)
            {
                MessageBox.Show("The file you provided is not a valid LearnEngine PAK file!", "PAK Viewer", MessageBoxButtons.OK, MessageBoxIcon.Error);
                return;
            }


            Text = $"{fileName} — PAK Viewer";
        }

        private void AboutToolStripMenuItem_Click(object sender, EventArgs e)
        {
            AboutBox aboutBox = new AboutBox();
            aboutBox.ShowDialog(this);
        }
    }
}
