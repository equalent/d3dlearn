using System;
using System.Collections.Generic;
using System.IO;
using System.Windows.Forms;

namespace PakViewer
{
    public partial class Form1 : Form
    {
        private Pak pak;
        private List<string> mArgs;
        private Dictionary<UInt64, string> mFileStrings;

        public Form1(string[] args)
        {
            InitializeComponent();
            mArgs = new List<string>(args);
            if(mArgs.Count >= 1)
            {
                string firstArg = mArgs[0];
                if (!firstArg.Trim().StartsWith("-"))
                {
                    LoadPAK(firstArg);
                }
            }
        }

        private void ExitToolStripMenuItem_Click(object sender, EventArgs e)
        {
            Close();
        }

        private void InitializeStrings(string stringsPath)
        {
            mFileStrings = new Dictionary<ulong, string>();
            string rawStrings = File.ReadAllText(stringsPath);
            string[] stringLines = rawStrings.Split(new[] { '&' }, StringSplitOptions.RemoveEmptyEntries);
            foreach(var line in stringLines)
            {
                string[] lineSubs = line.Split(new[] { '$' }, StringSplitOptions.RemoveEmptyEntries);
                UInt64 hash = UInt64.Parse(lineSubs[0]);
                string filename = lineSubs[1];
                mFileStrings.Add(hash, filename);
            }
        }

        private string GetFilename(UInt64 hash)
        {
            if (mFileStrings.ContainsKey(hash))
            {
                return mFileStrings[hash];
            } else
            {
                return "[FAILED TO GET FILENAME] " + hash.ToString();
            }
        }

        private void LoadPAK(string filePath)
        {
            string fileName = Path.GetFileName(filePath);

            bool doStrings;
            string stringsPath = null;

            DialogResult dialogResult = MessageBox.Show("Do you want to load .strings file? Without .string filenames will be replaced by their hashes", "PAK Viewer", MessageBoxButtons.YesNo, MessageBoxIcon.Question);
            if (dialogResult.Equals(DialogResult.Yes))
            {
                doStrings = openFileDialog2.ShowDialog() != DialogResult.Cancel;
                stringsPath = openFileDialog2.FileName;
            }
            else
            {
                doStrings = false;
            }

            try
            {
                pak = new Pak(filePath);
                toolStripStatusLabel1.Text = $"{pak.NumFiles} files in PAK";
                propertyGrid1.SelectedObject = pak;
            }
            catch (PakBadImageException)
            {
                MessageBox.Show("The file you provided is not a valid LearnEngine PAK file!", "PAK Viewer", MessageBoxButtons.OK, MessageBoxIcon.Error);
                return;
            }

            listView1.Items.Clear();
            if (!doStrings)
            {
                listView1.Columns[0].Text = "Filename hash";
            } else
            {
                listView1.Columns[0].Text = "Filename";
                InitializeStrings(stringsPath);
            }

            foreach(var file in pak.mPakInfo.files)
            {
                if (doStrings)
                {
                    listView1.Items.Add(new ListViewItem(new string[]
                    {
                        GetFilename(file.filenameHash),
                        Pak.SizeSuffix(file.size),
                        file.offset.ToString(),
                        file.hash.ToString()
                    }));
                } else
                {
                    listView1.Items.Add(new ListViewItem(new string[]
                    {
                        file.filenameHash.ToString(),
                        Pak.SizeSuffix(file.size),
                        file.offset.ToString(),
                        file.hash.ToString()
                    }));
                }
                
            }


            Text = $"{fileName} — PAK Viewer";
        }

        private void OpenToolStripMenuItem_Click(object sender, EventArgs e)
        {
            DialogResult dialogResult = openFileDialog1.ShowDialog();
            if (dialogResult.Equals(DialogResult.Cancel))
            {
                return;
            }
            LoadPAK(openFileDialog1.FileName);
        }

        private void AboutToolStripMenuItem_Click(object sender, EventArgs e)
        {
            AboutBox aboutBox = new AboutBox();
            aboutBox.ShowDialog(this);
        }
    }
}
