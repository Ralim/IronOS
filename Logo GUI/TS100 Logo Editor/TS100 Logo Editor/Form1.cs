using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace TS100_Logo_Editor
{
    public partial class Form1 : Form
    {
        public Form1()
        {
            InitializeComponent();
        }
        Image sourceImage;
        private void btnLoadImage_Click(object sender, EventArgs e)
        {
            //load in an image
            OpenFileDialog dlg = new OpenFileDialog();
            dlg.Title = "Select Image";
            if (dlg.ShowDialog() == DialogResult.OK)
            {
                //user has selected an image
                string filename = dlg.FileName;
                sourceImage = Image.FromFile(filename);
                reDrawPreview();
            }

        }

        private void reDrawPreview()
        {
            Bitmap b = new Bitmap(96, 16);
            //scale mode
            if (rbScaleFit.Checked)
            {
                //fit
                float scalefactor = Math.Min((float)b.Width / (float)sourceImage.Width, (float)b.Height / (float)sourceImage.Height);
                using (Graphics g = Graphics.FromImage(b))
                {
                    g.DrawImage(sourceImage, new RectangleF(0, 0, sourceImage.Width * scalefactor, sourceImage.Height * scalefactor));
                }
            }
            else
            {
                //draw image stretched
                using (Graphics g = Graphics.FromImage(b))
                {
                    g.DrawImage(sourceImage, new RectangleF(0, 0, 96, 16));
                }
            }
            //We now have our downsampled colour image
            //apply inversion
            if (cbInvertImage.Checked)
            {
                for (int y = 0; (y <= (b.Height - 1)); y++)
                {
                    for (int x = 0; (x <= (b.Width - 1)); x++)
                    {
                        Color inv = b.GetPixel(x, y);
                        inv = Color.FromArgb(255, (255 - inv.R), (255 - inv.G), (255 - inv.B));
                        b.SetPixel(x, y, inv);
                    }
                }
            }

            //Threshold image
            b = GrayScale(b);
            //draw image at 2x scale
            Bitmap bBig = new Bitmap(96 * 2, 16 * 2);
            using (Graphics g = Graphics.FromImage(bBig))
            {
                g.DrawImage(b, new RectangleF(0, 0, bBig.Width, bBig.Height));
            }
            pbImage.Image = bBig;
        }
        public Bitmap GrayScale(Bitmap Bmp)
        {
            int rgb;
            Color c;

            for (int y = 0; y < Bmp.Height; y++)
                for (int x = 0; x < Bmp.Width; x++)
                {
                    c = Bmp.GetPixel(x, y);
                    rgb = (int)((c.R + c.G + c.B) / 3);
                    if (rgb > 128)
                        rgb = 0xFF;
                    else
                        rgb = 0x00;
                    Bmp.SetPixel(x, y, Color.FromArgb(rgb, rgb, rgb));
                }
            return Bmp;
        }

        private void rbScaleStretch_CheckedChanged(object sender, EventArgs e)
        {
            reDrawPreview();
        }

        private void rbScaleFit_CheckedChanged(object sender, EventArgs e)
        {
            reDrawPreview();
        }

        private void cbInvertImage_CheckedChanged(object sender, EventArgs e)
        {
            reDrawPreview();
        }

        private void btnSaveHex_Click(object sender, EventArgs e)
        {
            Bitmap bmp = (Bitmap)pbImage.Image;
            //convert image to byte array
            byte[] data = new byte[96 * 16 / 8];
            for (int i = 0; i < data.Length; i++)
            {
                //loop through all the bytes
                byte b = 0;//local byte
                //i sets the starting column
                for (int y = 0; y < 8; y++)
                {
                    var px = bmp.GetPixel(i % 96, (i / 96) == 1 ? 8 + y : y);
                    //we loop down the picture
                    //LSB is the top, MSB is the bottom
                    if (px.R >= 128)
                    {
                        //pixel is white
                        b |= (byte)(1 << y);
                    }
                }
                data[i] = b;
            }

        }
    }
}
