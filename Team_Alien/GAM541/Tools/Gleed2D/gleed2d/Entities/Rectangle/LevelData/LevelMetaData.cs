using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Microsoft.Xna.Framework;
using Microsoft.Xna.Framework.Graphics;
using System.ComponentModel;
using System.IO;
using System.Xml;
using System.Xml.Serialization;

namespace GLEED2D
{
    public class rootMetaData:CircleItem
    {

        public Vector2 gravity;

        [DisplayName("Gravity"), Category(" root properties")]
        [XmlIgnore()]
       
        public Vector2 Gravity
        {
            get { return gravity; }
            set { gravity = value; }
        }
        public rootMetaData(Vector2 startpos, float radius)
            : base(startpos, radius)
        {
            gravity = new Vector2(0, 100);
        }
    }
}
