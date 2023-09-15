namespace GattSerialClient
{
    partial class fmMain
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
            this.btConnect = new System.Windows.Forms.Button();
            this.btDisconnect = new System.Windows.Forms.Button();
            this.laValue = new System.Windows.Forms.Label();
            this.edValue = new System.Windows.Forms.TextBox();
            this.btSend = new System.Windows.Forms.Button();
            this.lbLog = new System.Windows.Forms.ListBox();
            this.btClear = new System.Windows.Forms.Button();
            this.SuspendLayout();
            // 
            // btConnect
            // 
            this.btConnect.Location = new System.Drawing.Point(12, 12);
            this.btConnect.Name = "btConnect";
            this.btConnect.Size = new System.Drawing.Size(75, 23);
            this.btConnect.TabIndex = 0;
            this.btConnect.Text = "Connect";
            this.btConnect.UseVisualStyleBackColor = true;
            this.btConnect.Click += new System.EventHandler(this.btConnect_Click);
            // 
            // btDisconnect
            // 
            this.btDisconnect.Enabled = false;
            this.btDisconnect.Location = new System.Drawing.Point(93, 12);
            this.btDisconnect.Name = "btDisconnect";
            this.btDisconnect.Size = new System.Drawing.Size(75, 23);
            this.btDisconnect.TabIndex = 1;
            this.btDisconnect.Text = "Disconnect";
            this.btDisconnect.UseVisualStyleBackColor = true;
            this.btDisconnect.Click += new System.EventHandler(this.btDisconnect_Click);
            // 
            // laValue
            // 
            this.laValue.AutoSize = true;
            this.laValue.Location = new System.Drawing.Point(174, 17);
            this.laValue.Name = "laValue";
            this.laValue.Size = new System.Drawing.Size(171, 13);
            this.laValue.TabIndex = 2;
            this.laValue.Text = "Hexadecimal value (32 bytes max):";
            // 
            // edValue
            // 
            this.edValue.Location = new System.Drawing.Point(351, 14);
            this.edValue.Name = "edValue";
            this.edValue.Size = new System.Drawing.Size(272, 20);
            this.edValue.TabIndex = 3;
            this.edValue.Text = "11223344";
            this.edValue.KeyPress += new System.Windows.Forms.KeyPressEventHandler(this.edValue_KeyPress);
            // 
            // btSend
            // 
            this.btSend.Enabled = false;
            this.btSend.Location = new System.Drawing.Point(629, 12);
            this.btSend.Name = "btSend";
            this.btSend.Size = new System.Drawing.Size(75, 23);
            this.btSend.TabIndex = 4;
            this.btSend.Text = "Send";
            this.btSend.UseVisualStyleBackColor = true;
            this.btSend.Click += new System.EventHandler(this.btSend_Click);
            // 
            // lbLog
            // 
            this.lbLog.FormattingEnabled = true;
            this.lbLog.Location = new System.Drawing.Point(12, 41);
            this.lbLog.Name = "lbLog";
            this.lbLog.Size = new System.Drawing.Size(692, 472);
            this.lbLog.TabIndex = 5;
            // 
            // btClear
            // 
            this.btClear.Location = new System.Drawing.Point(629, 525);
            this.btClear.Name = "btClear";
            this.btClear.Size = new System.Drawing.Size(75, 23);
            this.btClear.TabIndex = 6;
            this.btClear.Text = "Clear";
            this.btClear.UseVisualStyleBackColor = true;
            this.btClear.Click += new System.EventHandler(this.btClear_Click);
            // 
            // fmMain
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(716, 560);
            this.Controls.Add(this.btClear);
            this.Controls.Add(this.lbLog);
            this.Controls.Add(this.btSend);
            this.Controls.Add(this.edValue);
            this.Controls.Add(this.laValue);
            this.Controls.Add(this.btDisconnect);
            this.Controls.Add(this.btConnect);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedSingle;
            this.Name = "fmMain";
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
            this.Text = "GATT Serial Client";
            this.FormClosed += new System.Windows.Forms.FormClosedEventHandler(this.fmMain_FormClosed);
            this.Load += new System.EventHandler(this.fmMain_Load);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Button btConnect;
        private System.Windows.Forms.Button btDisconnect;
        private System.Windows.Forms.Label laValue;
        private System.Windows.Forms.TextBox edValue;
        private System.Windows.Forms.Button btSend;
        private System.Windows.Forms.ListBox lbLog;
        private System.Windows.Forms.Button btClear;
    }
}

