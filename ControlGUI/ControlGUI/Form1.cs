using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.Diagnostics;

namespace ControlGUI
{
    public partial class Form1 : Form
    {
        int initX, initY, initHeight, initWidth;
        Rectangle targetRect;

        // -1: no change, 0 = in rect, 1 = left top, 2 = right top, 3 = left bottum, 4 = right bottum
        int mouseDownCorner = -1;
        int mLastX = 0;
        int mLastY = 0;

        // 1: normal, 2 = easy, 3 = intermediate, 4 = hard
        int mode = 1;
        int ballLeft = 0;
        int score = 0;
        int total = 0;
        int eHigh = 0;
        int iHigh = 0;
        int hHigh = 0;

        int timeLeft;
        Random rnd = new Random();

        Process cameraProcess = new Process();

        public Form1()
        {
            InitializeComponent();
            Height = Screen.FromControl(this).Bounds.Height;
            Width = Screen.FromControl(this).Bounds.Width;
            pictureBox1.Height = Height - 60;
            pictureBox1.Width = Width - 180;
            initX = pictureBox1.Width / 2 - 100;
            initY = pictureBox1.Height / 2 - 100;
            initHeight = 200;
            initWidth = 200;
            targetRect = new Rectangle(initX, initY, initHeight, initWidth);
            cameraProcess.StartInfo.FileName = @"C:\General use\Homework\CS 4710\Project1\x64\Debug\Project1.exe";
            cameraProcess.EnableRaisingEvents = true;
            // TODO read high score
            try {
                string[] lines = System.IO.File.ReadAllLines("scores.txt");
                eHigh = Int32.Parse(lines[0]);
                iHigh = Int32.Parse(lines[1]);
                hHigh = Int32.Parse(lines[2]);
                easyScoreLabel.Text = eHigh.ToString();
                intermediateScoreLabel.Text = iHigh.ToString();
                hardScoreLabel.Text = hHigh.ToString();
            }
            catch (Exception)
            { }
        }

        private void pictureBox1_Paint(object sender, PaintEventArgs e)
        {
            Pen pen = new Pen(Color.FromArgb(255,255,63), 5);
            e.Graphics.Clear(Color.FromArgb(0,0,192));
            e.Graphics.DrawRectangle(pen, targetRect);
        }

        private void pictureBox1_MouseDown(object sender, MouseEventArgs e)
        {
            if(!SettingCheckBox.Checked)
            {
                return;
            }
            if (e.Button == MouseButtons.Left)
            {
                if (near(e.X, e.Y, targetRect.X, targetRect.Y))
                {
                    mouseDownCorner = 1;
                }
                else if (near(e.X, e.Y, targetRect.X + targetRect.Width, targetRect.Y))
                {
                    mouseDownCorner = 2;
                }
                else if (near(e.X, e.Y, targetRect.X, targetRect.Y + targetRect.Height))
                {
                    mouseDownCorner = 3;
                }
                else if (near(e.X, e.Y, targetRect.X + targetRect.Width, targetRect.Y + targetRect.Height))
                {
                    mouseDownCorner = 4;
                }
                else if(e.X - targetRect.X < targetRect.Width && e.Y - targetRect.Y < targetRect.Height
                    && e.X > targetRect.X && e.Y > targetRect.Y)
                {
                    mouseDownCorner = 0;
                    mLastX = e.X;
                    mLastY = e.Y;
                }
            }
        }

        bool near(int x1, int y1, int x2, int y2)
        {
            int dx = x1 - x2;
            int dy = y1 - y2;
            return dx * dx + dy * dy < 100;
        }

        private void pictureBox1_MouseMove(object sender, MouseEventArgs e)
        {
            if (!SettingCheckBox.Checked)
            {
                return;
            }
            if (e.Button == MouseButtons.Left)
            {
                int dx = 0;
                int dy = 0;
                int dw = 0;
                int dh = 0;
                switch (mouseDownCorner)
                {
                    case 0:
                        targetRect.X += e.X - mLastX;
                        targetRect.Y += e.Y - mLastY;
                        break;
                    case 1:
                        dx = e.X - targetRect.X;
                        dy = e.Y - targetRect.Y;
                        if (dx > targetRect.Width - 50)
                        {
                            dx = targetRect.Width - 50;
                        }
                        targetRect.X += dx;
                        targetRect.Width -= dx;
                        if (dy > targetRect.Height - 50)
                        {
                            dy = targetRect.Height - 50;
                        }
                        targetRect.Y += dy;
                        targetRect.Height -= dy;
                        break;
                    case 2:
                        dw = targetRect.X + targetRect.Width - e.X;
                        dy = e.Y - targetRect.Y;
                        if (e.X < targetRect.X + 50)
                        {
                            dw = 0;
                        }
                        targetRect.Width -= dw;
                        if (dy > targetRect.Height - 50)
                        {
                            dy = targetRect.Height - 50;
                        }
                        targetRect.Y += dy;
                        targetRect.Height -= dy;
                        break;
                    case 3:
                        dx = e.X - targetRect.X;
                        dh = targetRect.Y + targetRect.Height - e.Y;
                        if (dx > targetRect.Width - 50)
                        {
                            dx = targetRect.Width - 50;
                        }
                        targetRect.X += dx;
                        targetRect.Width -= dx;
                        if (e.Y < targetRect.Y + 50)
                        {
                            dh = 0;
                        }
                        targetRect.Height -= dh;
                        break;
                    case 4:
                        dw = targetRect.X + targetRect.Width - e.X;
                        dh = targetRect.Y + targetRect.Height - e.Y;
                        if (e.X < targetRect.X + 50)
                        {
                            dw = 0;
                        }
                        targetRect.Width -= dw;
                        if (e.Y < targetRect.Y + 50)
                        {
                            dh = 0;
                        }
                        targetRect.Height -= dh;
                        break;
                    default:
                        break;
                }
                pictureBox1.Refresh();
                mLastX = e.X;
                mLastY = e.Y;
            }
        }

        private void pictureBox1_MouseUp(object sender, MouseEventArgs e)
        {
            if (!SettingCheckBox.Checked)
            {
                return;
            }
            if (e.Button == MouseButtons.Left)
            {
                mouseDownCorner = -1;
            }
        }

        private void SettingCheckBox_CheckedChanged(object sender, EventArgs e)
        {
            startButton.Enabled = !SettingCheckBox.Checked;
            easyButton.Enabled = !SettingCheckBox.Checked;
            intermediateButton.Enabled = !SettingCheckBox.Checked;
            hardButton.Enabled = !SettingCheckBox.Checked;
        }

        private void startButton_Click(object sender, EventArgs e)
        {
            resetScore();
            mode = 1;
            startPitching();
        }

        private async void timer1_Tick(object sender, EventArgs e)
        {
            timeLeft = timeLeft - 1;
            countLabel.Text = timeLeft.ToString();
            if (timeLeft == 0)
            {
                timer1.Stop();
                processingLabel.Visible = true;
                countLabel.Text = "";

                await Task.Run(()=>cameraProcess.WaitForExit());

                processingLabel.Visible = false;

                if (mode != 1)
                {
                    //TODO read score
                    bool ballIn = rnd.Next(2) == 0;
                    float speed = 30f;
                    score = (int)(mode * mode * speed * (ballIn ? 100 : 1));
                    total += score;
                    scoreLabel.Text = score.ToString();
                    totalLabel.Text = total.ToString();

                    ballLeft--;
                    ballCountLabel.Text = ballLeft.ToString();
                    if (ballLeft > 0)
                    {
                        nextButton.Enabled = true;
                        return;
                    }
                    else
                    {
                        bool isHighScore = false;
                        switch (mode)
                        {
                            case 2:
                                if (total > eHigh)
                                {
                                    eHigh = total;
                                    easyScoreLabel.Text = eHigh.ToString();
                                    isHighScore = true;
                                }
                                break;
                            case 3:
                                if (total > iHigh)
                                {
                                    iHigh = total;
                                    intermediateScoreLabel.Text = iHigh.ToString();
                                    isHighScore = true;
                                }
                                break;
                            case 4:
                                if (total > hHigh)
                                {
                                    hHigh = total;
                                    hardScoreLabel.Text = hHigh.ToString();
                                    isHighScore = true;
                                }
                                break;
                            default:
                                break;
                        }
                        if (isHighScore)
                        {
                            using (System.IO.StreamWriter file = new System.IO.StreamWriter("scores.txt"))
                            {
                                file.WriteLine(eHigh);
                                file.WriteLine(iHigh);
                                file.WriteLine(hHigh);
                            }
                        }
                        targetRect.X = initX;
                        targetRect.Y = initY;
                        targetRect.Height = initHeight;
                        targetRect.Width = initWidth;
                        pictureBox1.Refresh();
                    }
                }
                else
                {
                    Process proc = new Process();
                    proc.StartInfo.FileName = @"C:\General use\Homework\CS 4710\WindowsGame1\WindowsGame1\WindowsGame1\bin\x86\Debug\WindowsGame1.exe";
                    proc.EnableRaisingEvents = true;
                    proc.Start();
                }
                enableButtons(true);
            }
        }

        void enableButtons(bool enable)
        {
            SettingCheckBox.Enabled = enable;
            easyButton.Enabled = enable;
            intermediateButton.Enabled = enable;
            hardButton.Enabled = enable;
            startButton.Enabled = enable;
        }

        void startPitching()
        {
            enableButtons(false);
            timer1.Start();
            timeLeft = 5;
            countLabel.Text = timeLeft.ToString();
            cameraProcess.Start();
        }

        void resetScore()
        {
            score = 0;
            total = 0;
            scoreLabel.Text = score.ToString();
            totalLabel.Text = total.ToString();
        }

        void startCompetition()
        {
            startPitching();
            ballCountLabel.Text = ballLeft.ToString();

            int gridX = rnd.Next(mode);
            int gridY = rnd.Next(mode);
            int unit = pictureBox1.Height / mode;
            int x = (pictureBox1.Width - pictureBox1.Height) / 2 + unit * gridX;
            int y = unit * gridY;
            targetRect.X = x;
            targetRect.Y = y;
            targetRect.Width = unit;
            targetRect.Height = unit;
            pictureBox1.Refresh();
        }

        private void easyButton_Click(object sender, EventArgs e)
        {
            resetScore();
            ballLeft = 5;
            mode = 2;
            startCompetition();
        }

        private void intermediateButton_Click(object sender, EventArgs e)
        {
            resetScore();
            ballLeft = 5;
            mode = 3;
            startCompetition();
        }

        private void hardButton_Click(object sender, EventArgs e)
        {
            resetScore();
            ballLeft = 5;
            mode = 4;
            startCompetition();
        }

        private void nextButton_Click(object sender, EventArgs e)
        {
            nextButton.Enabled = false;
            startCompetition();
        }
    }
}
