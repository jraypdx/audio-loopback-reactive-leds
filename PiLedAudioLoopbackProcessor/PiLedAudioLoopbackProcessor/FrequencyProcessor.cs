using NAudio.CoreAudioApi;
using NAudio.Wave;
using System;
using System.Numerics;
using MathNet.Numerics.IntegralTransforms;
using System.Collections.Generic;
using System.Linq;

namespace PiLedAudioLoopbackProcessor
{
    enum SmoothType
    {
        horizontal,
        vertical,
        both
    }

    /*
     * Visualizer using frequencies
     */
    class FrequencyProcessor
    {
        private WaveBuffer buffer;

        private static int vertical_smoothness = 1;
        private static int horizontal_smoothness = 1;  // ALWAYS LEAVE AT 0 it makes it much less precise
        private float size = 10;

        private int vis_mode = 0;

        private static SmoothType smoothType = SmoothType.both;

        private List<Complex[]> smooth = new List<Complex[]>();

        private Complex[] values;

        private double[] toSend;

        private double pre_value = 0;

        private double count = 32;

        public void Load()
        {
            toSend = new double[(int)count];

            // start audio capture
            var capture = new WasapiLoopbackCapture();

            capture.DataAvailable += DataAvailable;

            capture.RecordingStopped += (s, a) =>
            {
                capture.Dispose();
            };

            capture.StartRecording();
        }

        public void DataAvailable(object sender, WaveInEventArgs e)
        {
            buffer = new WaveBuffer(e.Buffer); // save the buffer in the class variable

            int len = buffer.FloatBuffer.Length / 8;

            // fft
            values = new Complex[len];
            for (int i = 0; i < len; i++)
                values[i] = new Complex(buffer.FloatBuffer[i], 0.0);
            Fourier.Forward(values, FourierOptions.Default);

            //FrequencySplitter.ProcessBins(values.Select(x => x.Magnitude).ToArray());
            //shift array
            if (smoothType == SmoothType.vertical || smoothType == SmoothType.both)
            {
                smooth.Add(values);
                if (smooth.Count > vertical_smoothness)
                    smooth.RemoveAt(0);
            }
            FrequencySplitter.ProcessBins(values.Select(x => x.Magnitude).ToArray());
            //Draw();
            //foreach (var ts in toSend)
            //{
            //    Console.Write(ts.ToString());
            //}
            //Console.WriteLine("---");
        }

        public double vSmooth(int i, Complex[][] s)
        {
            double value = 0;

            for (int v = 0; v < s.Length; v++)
                value += Math.Abs(s[v] != null ? s[v][i].Magnitude : 0.0);

            return value / s.Length;
        }

        public double MovingAverage(Complex[] v, int i)
        {
            double value = 0;

            for (int h = Math.Max(i - horizontal_smoothness, 0); h < Math.Min(i + horizontal_smoothness, count); h++)
                value += v[h].Magnitude;

            return value / ((horizontal_smoothness + 1) * 2);
        }

        public double BothSmooth(int i)
        {
            var s = smooth.ToArray();

            double value = 0;

            for (int h = Math.Max(i - horizontal_smoothness, 0); h < Math.Min(i + horizontal_smoothness, count); h++)
                value += vSmooth(h, s);

            return value / ((horizontal_smoothness + 1) * 2);
        }

        public double hSmooth(int i)
        {
            if (i > 1)
            {
                double value = values[i].Magnitude;

                for (int h = i - horizontal_smoothness; h <= i + horizontal_smoothness; h++)
                    value += values[h].Magnitude;

                return value / ((horizontal_smoothness + 1) * 2);
            }

            return 0;
        }

        public void Draw()
        {
            if (smoothType == SmoothType.vertical)
            {
                var s = smooth.ToArray();
                // vertical smoothness
                for (int i = 0; i < count; i++)
                {
                    double value = 0;
                    for (int v = 0; v < s.Length; v++)
                        value += Math.Abs(s[v] != null ? s[v][i].Magnitude : 0.0);
                    value /= s.Length;

                    toSend[i] = value;
                }
            }
            else if (smoothType == SmoothType.horizontal)
            {
                for (int i = 0; i < count; i++)
                {
                    double value = 0;
                    for (int h = Math.Max(i - horizontal_smoothness, 0); h < Math.Min(i + horizontal_smoothness, count); h++)
                        value += values[h].Magnitude;
                    value /= ((horizontal_smoothness + 1) * 2);

                    toSend[i] = value;
                }
            }
            else if (smoothType == SmoothType.both)
            {
                for (int i = 0; i < count; i++)
                {
                    double value = BothSmooth(i);

                    toSend[i] = value;
                }
            }

            FrequencySplitter.ProcessBins(toSend);
        }
    }
}

