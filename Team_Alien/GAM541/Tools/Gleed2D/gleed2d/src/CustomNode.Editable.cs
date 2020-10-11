using System;
using System.IO;
using System.Collections.Generic;
using System.Xml.Serialization;
using System.Windows.Forms;
using Microsoft.Xna.Framework;
using Microsoft.Xna.Framework.Graphics;
using System.Xml;
using System.Text;
using System.ComponentModel;


namespace GLEED2D
{
    public partial class CustomNode
    {

        [XmlIgnore]
        public Item ItemParent;

    }
}
