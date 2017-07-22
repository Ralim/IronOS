namespace TS100_Logo_Editor
{
    partial class Form1
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.groupBox1 = new System.Windows.Forms.GroupBox();
            this.groupBox2 = new System.Windows.Forms.GroupBox();
            this.groupBox3 = new System.Windows.Forms.GroupBox();
            this.btnLoadImage = new System.Windows.Forms.Button();
            this.btnSaveHex = new System.Windows.Forms.Button();
            this.pbImage = new System.Windows.Forms.PictureBox();
            this.gbScaling = new System.Windows.Forms.GroupBox();
            this.rbScaleFit = new System.Windows.Forms.RadioButton();
            this.rbScaleStretch = new System.Windows.Forms.RadioButton();
            this.cbInvertImage = new System.Windows.Forms.CheckBox();
            this.groupBox1.SuspendLayout();
            this.groupBox2.SuspendLayout();
            this.groupBox3.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.pbImage)).BeginInit();
            this.gbScaling.SuspendLayout();
            this.SuspendLayout();
            // 
            // groupBox1
            // 
            this.groupBox1.Controls.Add(this.btnLoadImage);
            this.groupBox1.Location = new System.Drawing.Point(13, 13);
            this.groupBox1.Name = "groupBox1";
            this.groupBox1.Size = new System.Drawing.Size(200, 141);
            this.groupBox1.TabIndex = 0;
            this.groupBox1.TabStop = false;
            this.groupBox1.Text = "1. Load source Image";
            // 
            // groupBox2
            // 
            this.groupBox2.Controls.Add(this.cbInvertImage);
            this.groupBox2.Controls.Add(this.gbScaling);
            this.groupBox2.Controls.Add(this.pbImage);
            this.groupBox2.Location = new System.Drawing.Point(220, 13);
            this.groupBox2.Name = "groupBox2";
            this.groupBox2.Size = new System.Drawing.Size(200, 141);
            this.groupBox2.TabIndex = 1;
            this.groupBox2.TabStop = false;
            this.groupBox2.Text = "2. Adjust image";
            // 
            // groupBox3
            // 
            this.groupBox3.Controls.Add(this.btnSaveHex);
            this.groupBox3.Location = new System.Drawing.Point(427, 13);
            this.groupBox3.Name = "groupBox3";
            this.groupBox3.Size = new System.Drawing.Size(200, 141);
            this.groupBox3.TabIndex = 2;
            this.groupBox3.TabStop = false;
            this.groupBox3.Text = "3. Export";
            // 
            // btnLoadImage
            // 
            this.btnLoadImage.Location = new System.Drawing.Point(7, 20);
            this.btnLoadImage.Name = "btnLoadImage";
            this.btnLoadImage.Size = new System.Drawing.Size(187, 23);
            this.btnLoadImage.TabIndex = 0;
            this.btnLoadImage.Text = "Load Image";
            this.btnLoadImage.UseVisualStyleBackColor = true;
            this.btnLoadImage.Click += new System.EventHandler(this.btnLoadImage_Click);
            // 
            // btnSaveHex
            // 
            this.btnSaveHex.Location = new System.Drawing.Point(7, 20);
            this.btnSaveHex.Name = "btnSaveHex";
            this.btnSaveHex.Size = new System.Drawing.Size(187, 23);
            this.btnSaveHex.TabIndex = 0;
            this.btnSaveHex.Text = "Save DFU File";
            this.btnSaveHex.UseVisualStyleBackColor = true;
            this.btnSaveHex.Click += new System.EventHandler(this.btnSaveHex_Click);
            // 
            // pbImage
            // 
            this.pbImage.BackColor = System.Drawing.Color.Black;
            this.pbImage.Location = new System.Drawing.Point(3, 20);
            this.pbImage.Name = "pbImage";
            this.pbImage.Size = new System.Drawing.Size(192, 32);
            this.pbImage.TabIndex = 0;
            this.pbImage.TabStop = false;
            // 
            // gbScaling
            // 
            this.gbScaling.Controls.Add(this.rbScaleStretch);
            this.gbScaling.Controls.Add(this.rbScaleFit);
            this.gbScaling.Location = new System.Drawing.Point(6, 59);
            this.gbScaling.Name = "gbScaling";
            this.gbScaling.Size = new System.Drawing.Size(80, 76);
            this.gbScaling.TabIndex = 1;
            this.gbScaling.TabStop = false;
            this.gbScaling.Text = "Scaling";
            // 
            // rbScaleFit
            // 
            this.rbScaleFit.AutoSize = true;
            this.rbScaleFit.Checked = true;
            this.rbScaleFit.Location = new System.Drawing.Point(7, 20);
            this.rbScaleFit.Name = "rbScaleFit";
            this.rbScaleFit.Size = new System.Drawing.Size(36, 17);
            this.rbScaleFit.TabIndex = 0;
            this.rbScaleFit.TabStop = true;
            this.rbScaleFit.Text = "Fit";
            this.rbScaleFit.UseVisualStyleBackColor = true;
            this.rbScaleFit.CheckedChanged += new System.EventHandler(this.rbScaleFit_CheckedChanged);
            // 
            // rbScaleStretch
            // 
            this.rbScaleStretch.AutoSize = true;
            this.rbScaleStretch.Location = new System.Drawing.Point(7, 44);
            this.rbScaleStretch.Name = "rbScaleStretch";
            this.rbScaleStretch.Size = new System.Drawing.Size(59, 17);
            this.rbScaleStretch.TabIndex = 1;
            this.rbScaleStretch.Text = "Stretch";
            this.rbScaleStretch.UseVisualStyleBackColor = true;
            this.rbScaleStretch.CheckedChanged += new System.EventHandler(this.rbScaleStretch_CheckedChanged);
            // 
            // cbInvertImage
            // 
            this.cbInvertImage.AutoSize = true;
            this.cbInvertImage.Location = new System.Drawing.Point(92, 79);
            this.cbInvertImage.Name = "cbInvertImage";
            this.cbInvertImage.Size = new System.Drawing.Size(53, 17);
            this.cbInvertImage.TabIndex = 2;
            this.cbInvertImage.Text = "Invert";
            this.cbInvertImage.UseVisualStyleBackColor = true;
            this.cbInvertImage.CheckedChanged += new System.EventHandler(this.cbInvertImage_CheckedChanged);
            // 
            // Form1
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(642, 172);
            this.Controls.Add(this.groupBox3);
            this.Controls.Add(this.groupBox2);
            this.Controls.Add(this.groupBox1);
            this.DoubleBuffered = true;
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog;
            this.Name = "Form1";
            this.ShowIcon = false;
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
            this.Text = "TS100 Custom Logo editor";
            this.groupBox1.ResumeLayout(false);
            this.groupBox2.ResumeLayout(false);
            this.groupBox2.PerformLayout();
            this.groupBox3.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.pbImage)).EndInit();
            this.gbScaling.ResumeLayout(false);
            this.gbScaling.PerformLayout();
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.GroupBox groupBox1;
        private System.Windows.Forms.Button btnLoadImage;
        private System.Windows.Forms.GroupBox groupBox2;
        private System.Windows.Forms.CheckBox cbInvertImage;
        private System.Windows.Forms.GroupBox gbScaling;
        private System.Windows.Forms.RadioButton rbScaleStretch;
        private System.Windows.Forms.RadioButton rbScaleFit;
        private System.Windows.Forms.PictureBox pbImage;
        private System.Windows.Forms.GroupBox groupBox3;
        private System.Windows.Forms.Button btnSaveHex;
    }
}

