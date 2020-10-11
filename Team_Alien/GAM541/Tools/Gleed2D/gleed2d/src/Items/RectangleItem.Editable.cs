using System;
using System.ComponentModel;
using System.Xml;
using System.Xml.Schema;
using System.Xml.Serialization;
using Microsoft.Xna.Framework;
using Microsoft.Xna.Framework.Graphics;
using System.Collections.Generic;
using CustomUITypeEditors;
using System.Drawing.Design;
using System.Windows.Forms;



namespace GLEED2D
{

    public partial class RectangleItem
    {
        enum EdgeEnum
        {
            none, left, right, top, bottom
        }

        EdgeEnum edgeundermouse, edgegrabbed;

        [DisplayName("Width"), Category(" General")]
        [XmlIgnore()]
        public float pWidth
        {
            get
            {
                return Width;
            }
            set
            {
                Width = value;
                OnTransformed();
            }
        }

        [DisplayName("Height"), Category(" General")]
        [XmlIgnore()]
        public float pHeight
        {
            get
            {
                return Height;
            }
            set
            {
                Height = value;
                OnTransformed();
            }
        }


        [DisplayName("Scale"), Category(" General")]
        [XmlIgnore()]
        public string pScale { get { return Scale.value ;  }  }

        [DisplayName("BodyScale"), Category(" General")]
        [XmlIgnore()]
        public string pBodyScale { get { return BodyScale.value; } set { BodyScale.value = value; } }

        [DisplayName("ManualBodyScale"), Category(" General")]
        [XmlIgnore()]
        public string pManualBodyScale { get { return ManualBodyScale.value; } set { ManualBodyScale.value = value; } } 

        
        [DisplayName("ResetOnLoad"), Category(" General")]
        [XmlIgnore()]
        public string pResetOnLoad { get { return ResetOnLoad.value; } set { ResetOnLoad.value = value; } } 

        [DisplayName("FillColor"), Category(" General")]
        [Editor(typeof(XNAColorUITypeEditor), typeof(UITypeEditor))]
        [XmlIgnore()]
        public Color pFillColor { get { return FillColor; } set { FillColor = value; } }

        Rectangle Rectangle;
        int initialwidth, initialheight;

        public RectangleItem(Rectangle rect) : base()
        {
            PositionOrig = rect.Location.ToVector2();
            Width = rect.Width;
            Height = rect.Height;
            PositionOrig.X += (Width / 2);
            PositionOrig.Y += (Height/2 );
            Scale = new CustomNode("string");
            BodyScale = new CustomNode("string");
            ResetOnLoad = new CustomNode("string");
            ManualBodyScale = new CustomNode("string");
            ArchetypeId = new CustomNode("string");
            Rotation.value = "0";
            OnTransformed();
            FillColor = Constants.Instance.ColorPrimitives;

        }

        public override Item clone()
        {
            RectangleItem result = (RectangleItem)this.MemberwiseClone();
            result.CustomProperties = new SerializableDictionary(CustomProperties);
            result.Scale = new CustomNode("string");
            result.BodyScale = new CustomNode("string");
            result.ManualBodyScale.value = this.ManualBodyScale.value;
            result.Rotation = new CustomNode("string");
            result.Position = new CustomNode("string");
            result.ResetOnLoad = new CustomNode("string");
            result.SecondaryObjectId = new CustomNode("string");
            result.ActivatesObjectWithSecondaryId = new CustomNode("string");
            result.ActivatedBySecondaryObjectIds = new CustomNode("string");
            ArchetypeId  = new CustomNode("string");
            result.hovering = false;
            result.OnTransformed();
            return result;
        }

        public override string getNamePrefix()
        {
            return "Rectangle_";
        }

        public override bool contains(Vector2 worldpos)
        {
            return Rectangle.Contains(new Point((int)worldpos.X, (int)worldpos.Y));
        }


        public override void OnTransformed()
        {
            Vector2 temp;
            Rectangle.Location = PositionOrig.ToPoint();
            Rectangle.Width = (int)Width;
            Rectangle.Height = (int)Height;
            temp=PositionOrig;
            temp.X -= (Rectangle.Width / 2);
            temp.Y -= (Rectangle.Height / 2);

            Rectangle.Location = temp.ToPoint();
            Scale.value = (Width / scaleRatio).ToString() + "," + (Height / scaleRatio).ToString() + ",1";
            if (ManualBodyScale.value == "")
            {
                BodyScale.value = (Width / scaleRatio / 2.0).ToString() + "," + (Height / scaleRatio / 2.0).ToString();
            }
            else
            {
                BodyScale.value = ManualBodyScale.value;
            }

            Position.value = (PositionOrig.X / positionRatio).ToString() + "," + (-PositionOrig.Y / positionRatio).ToString();
        }

        public override void onMouseOver(Vector2 mouseworldpos)
        {
            //System.Diagnostics.Debug.WriteLine(System.DateTime.Now.ToString() + "RectangleItem.onMouseOver()");

            int edgewidth = 10;
            if (Math.Abs(mouseworldpos.X - Rectangle.Left) <= edgewidth)
            {
                MainForm.Instance.pictureBox1.Cursor = Cursors.SizeWE;
                edgeundermouse = EdgeEnum.left;
            }
            else if (Math.Abs(mouseworldpos.X - Rectangle.Right) <= edgewidth)
            {
                MainForm.Instance.pictureBox1.Cursor = Cursors.SizeWE;
                edgeundermouse = EdgeEnum.right;
            }
            else if (Math.Abs(mouseworldpos.Y - Rectangle.Top) <= edgewidth)
            {
                MainForm.Instance.pictureBox1.Cursor = Cursors.SizeNS;
                edgeundermouse = EdgeEnum.top;
            }
            else if (Math.Abs(mouseworldpos.Y - Rectangle.Bottom) <= edgewidth)
            {
                MainForm.Instance.pictureBox1.Cursor = Cursors.SizeNS;
                edgeundermouse = EdgeEnum.bottom;
            }
            else
            {
                MainForm.Instance.pictureBox1.Cursor = Cursors.Default;
                edgeundermouse = EdgeEnum.none;
            }
            base.onMouseOver(mouseworldpos);
        }

        public override void onMouseOut()
        {
            //System.Diagnostics.Debug.WriteLine(System.DateTime.Now.ToString() + "RectangleItem.onMouseOut()");
            base.onMouseOut();
        }

        public override void onMouseButtonDown(Vector2 mouseworldpos)
        {
            hovering = false;
            if (edgeundermouse != EdgeEnum.none)
            {
                edgegrabbed = edgeundermouse;
                initialwidth = Rectangle.Width;
                initialheight = Rectangle.Height;
            }
            else MainForm.Instance.pictureBox1.Cursor = Cursors.SizeAll;
            base.onMouseButtonDown(mouseworldpos);
        }

        public override void onMouseButtonUp(Vector2 mouseworldpos)
        {
            edgegrabbed = EdgeEnum.none;
            base.onMouseButtonUp(mouseworldpos);
        }

        public override void setPosition(Vector2 pos)
        {
            Vector2 delta = pos - PositionOrig;
            if (pos == PositionOrig) return;
            switch (edgegrabbed)
            {
                case EdgeEnum.left:
                    PositionOrig.X = pos.X;
                    Width -= (int)delta.X;
                    OnTransformed();
                    break;
                case EdgeEnum.right:
                    Width = initialwidth + (int)delta.X;
                    OnTransformed();
                    break;
                case EdgeEnum.top:
                    PositionOrig.Y = pos.Y;
                    Height -= (int)delta.Y;
                    OnTransformed();
                    break;
                case EdgeEnum.bottom:
                    Height = initialheight + (int)delta.Y;
                    OnTransformed();
                    break;
                case EdgeEnum.none:
                    base.setPosition(pos);
                    break;
            }
        }

        public override bool CanScale()
        {
            return true;
        }

        public override Vector2 getScale()
        {
            return new Vector2(pWidth, pWidth);
        }

        public override void setScale(Vector2 scale)
        {
            float factor = scale.X / pWidth;
            Width = (float)Math.Round(scale.X);
            Height = (float)Math.Round(Height * factor);
            Scale.value = (Width / scaleRatio).ToString() + "," + (Height / scaleRatio).ToString();
            OnTransformed();
        }

        public override void drawInEditor(SpriteBatch sb)
        {
            if (!Visible) return;

            Color c = FillColor;
            if (hovering && Constants.Instance.EnableHighlightOnMouseOver) c = Constants.Instance.ColorHighlight;
            Primitives.Instance.drawBoxFilled(sb, Rectangle, c);
        }


        public override void drawSelectionFrame(SpriteBatch sb, Matrix matrix, Color color)
        {
            
            Primitives.Instance.drawBox(sb, this.Rectangle.Transform(matrix), color, 2);
            
            Vector2[] poly = Rectangle.Transform(matrix).ToPolygon();

            foreach (Vector2 p in poly)
            {
                Primitives.Instance.drawCircleFilled(sb, p, 4, color);
            }

            Primitives.Instance.drawBoxFilled(sb, poly[0].X - 5, poly[0].Y - 5, 10, 10, color);


            
        }

    }
}
