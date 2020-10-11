using System;
using System.Windows.Forms;
using Microsoft.Xna.Framework;
using Microsoft.Xna.Framework.Graphics;

namespace GLEED2D
{
    public partial class AddCustomNode : Form
    {
        public AddCustomNode(MainForm main)
        {
            InitializeComponent();
        }

        private void buttonOK_Click(object sender, EventArgs e)
        {

            
        }

        private void buttonCancel_Click(object sender, EventArgs e)
        {
            this.Close();
        }


    }
}
