
using System;
using System.Drawing;
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
        const int LCDWidth = 96;
        Bitmap PureBMP;
        private void btnLoadImage_Click(object sender, EventArgs e)
        {
            //load in an image
            OpenFileDialog dlg = new OpenFileDialog();
            dlg.Title = "Select Image";
            dlg.Filter = "Image files (*.jpg, *.jpeg, *.bmp, *.png) | *.jpg; *.jpeg; *.bmp; *.png";
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
            PureBMP = new Bitmap(LCDWidth, 16);
            //scale mode
            if (rbScaleFit.Checked)
            {
                //fit
                float scalefactor = Math.Min((float)PureBMP.Width / (float)sourceImage.Width, (float)PureBMP.Height / (float)sourceImage.Height);
                using (Graphics g = Graphics.FromImage(PureBMP))
                {
                    g.DrawImage(sourceImage, new RectangleF(0, 0, sourceImage.Width * scalefactor, sourceImage.Height * scalefactor));
                }
            }
            else
            {
                //draw image stretched
                using (Graphics g = Graphics.FromImage(PureBMP))
                {
                    g.DrawImage(sourceImage, new RectangleF(0, 0, LCDWidth, 16));
                }
            }
            //We now have our downsampled colour image
            //apply inversion
            if (cbInvertImage.Checked)
            {
                for (int y = 0; (y <= (PureBMP.Height - 1)); y++)
                {
                    for (int x = 0; (x <= (PureBMP.Width - 1)); x++)
                    {
                        Color inv = PureBMP.GetPixel(x, y);
                        inv = Color.FromArgb(255, (255 - inv.R), (255 - inv.G), (255 - inv.B));
                        PureBMP.SetPixel(x, y, inv);
                    }
                }
            }

            //Threshold image
            PureBMP = GrayScale(PureBMP);
            //draw image at 2x scale
            Bitmap bBig = new Bitmap(pbImage.Width, pbImage.Height);
            using (Graphics g = Graphics.FromImage(bBig))
            {
                g.DrawImage(PureBMP, new RectangleF(0, 0, pbImage.Width, pbImage.Height));
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
            Bitmap bmp = PureBMP;
            //convert image to byte array
            byte[] data = new byte[1024];
            data[0] = 0xAA;
            data[1] = 0x55;
            data[2] = 0xF0;
            data[3] = 0x0D;

            for (int i = 0; i < (LCDWidth * 16 / 8); i++)
            {
                //loop through all the bytes
                byte b = 0;//local byte
                //i sets the starting column
                for (int y = 0; y < 8; y++)
                {
                    var px = bmp.GetPixel(i % LCDWidth, (i / LCDWidth) == 1 ? 8 + y : y);
                    //we loop down the picture
                    //LSB is the top, MSB is the bottom
                    if (px.R >= 128)
                    {
                        //pixel is white
                        b |= (byte)(1 << y);
                    }
                }
                data[i + 4] = b;
            }
            //We should now have the byte array that represents the image in the LCD format.
            //We now send this off to be encoded by the Intel Encoder
            //Flip all uint16_t pairs
            for (int i = 0; i < data.Length; i += 2)
            {
                //we need to swap each pair
                byte temp = data[i];
                data[i] = data[i + 1];
                data[i + 1] = temp;
            }
            string outputHexFile = IntelHex.IntelHex.encode(data, 0x0800F800, 16, true, true);//16 bytes is the only format the DFU seems to support //0x0800F800
            //^ This string now just needs to be written out to a text file :)
            SaveFileDialog dlg = new SaveFileDialog();
            dlg.Title = "Save DFU File";
            dlg.AddExtension = true;
            dlg.DefaultExt = ".hex";
            dlg.Filter = "Hex Files(*.hex)|*.hex";
            if (dlg.ShowDialog() == DialogResult.OK)
            {
                //The user has selected where they want to save the file
                using (var fs = new System.IO.StreamWriter(dlg.FileName))
                {
                    fs.Write(outputHexFile);
                }
            }

        }
    }
}
