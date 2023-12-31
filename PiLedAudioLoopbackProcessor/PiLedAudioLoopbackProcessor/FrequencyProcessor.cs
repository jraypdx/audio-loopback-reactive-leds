using NAudio.CoreAudioApi;
using NAudio.Wave;
using System;
using System.Numerics;
using MathNet.Numerics.IntegralTransforms;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;

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
            //Console.WriteLine(capture.)

            capture.DataAvailable += DataAvailable;

            capture.RecordingStopped += (s, a) =>
            {
                capture.Dispose();
            };

            capture.StartRecording();
        }

        public async void DataAvailable(object sender, WaveInEventArgs e)
        {
            buffer = new WaveBuffer(e.Buffer); // save the buffer in the class variable

            int len = buffer.FloatBuffer.Length / 8;
            var ms = 20;

            // fft
            values = new Complex[len];
            for (int i = 0; i < len; i++)
                values[i] = new Complex(buffer.FloatBuffer[i], 0.0);
            var chunk = len / 2;
            var v1 = values.Take(chunk).ToArray();
            var v2 = values.Skip(chunk).Take(chunk).ToArray();
            //var v3 = values.Skip(chunk * 2).Take(chunk).ToArray();
            //var v4 = values.Skip(chunk * 3).Take(chunk).ToArray();
            //var v5 = values.Skip(chunk * 4).Take(chunk).ToArray();
            //var v6 = values.Skip(chunk * 5).Take(chunk).ToArray();
            //var v7 = values.Skip(chunk * 6).Take(chunk).ToArray();
            //var v8 = values.Skip(chunk * 7).Take(chunk).ToArray();
            //Fourier.Forward(values, FourierOptions.InverseExponent);
            Fourier.Forward(v1, FourierOptions.InverseExponent);
            FrequencySplitterSerial.ProcessBins_RING(v1.Select(x => x.Magnitude).ToArray());
            await Task.Delay(ms);
            Fourier.Forward(v2, FourierOptions.InverseExponent);
            FrequencySplitterSerial.ProcessBins_RING(v2.Select(x => x.Magnitude).ToArray());
            await Task.Delay(ms);
            //Fourier.Forward(v3, FourierOptions.InverseExponent);
            //FrequencySplitterSerial.ProcessBins_RING(v3.Select(x => x.Magnitude).ToArray());
            //await Task.Delay(ms);
            //Fourier.Forward(v4, FourierOptions.InverseExponent);
            //FrequencySplitterSerial.ProcessBins_RING(v4.Select(x => x.Magnitude).ToArray());
            //await Task.Delay(ms);
            //Fourier.Forward(v5, FourierOptions.InverseExponent);
            //FrequencySplitterSerial.ProcessBins_RING(v5.Select(x => x.Magnitude).ToArray());
            //await Task.Delay(ms);
            //Fourier.Forward(v6, FourierOptions.InverseExponent);
            //FrequencySplitterSerial.ProcessBins_RING(v6.Select(x => x.Magnitude).ToArray());
            //await Task.Delay(ms);
            //Fourier.Forward(v7, FourierOptions.InverseExponent);
            //FrequencySplitterSerial.ProcessBins_RING(v7.Select(x => x.Magnitude).ToArray());
            //await Task.Delay(ms);
            //Fourier.Forward(v8, FourierOptions.InverseExponent);
            //FrequencySplitterSerial.ProcessBins_RING(v8.Select(x => x.Magnitude).ToArray());
            //await Task.Delay(ms);

            //FrequencySplitter.ProcessBins(values.Select(x => x.Magnitude).ToArray());
            //shift array
            if (smoothType == SmoothType.vertical || smoothType == SmoothType.both)
            {
                smooth.Add(values);
                if (smooth.Count > vertical_smoothness)
                    smooth.RemoveAt(0);
            }

            //var mag = values.Select(x => x.Magnitude);
            //var chunkSize = mag.Count() / 2;
            //var x1 = mag.Take(chunkSize).ToArray();
            //var x2 = mag.Skip(chunkSize).Take(chunkSize).ToArray();
            //var x3 = mag.Skip(chunkSize * 2).Take(chunkSize).ToArray();
            //var x4 = mag.Skip(chunkSize * 3).Take(chunkSize).ToArray();
            //var x5 = mag.Skip(chunkSize * 4).Take(chunkSize).ToArray();
            //var x6 = mag.Skip(chunkSize * 5).Take(chunkSize).ToArray();
            //var x7 = mag.Skip(chunkSize * 6).Take(chunkSize).ToArray();
            //var x8 = mag.Skip(chunkSize * 7).Take(chunkSize).ToArray();
            //FrequencySplitterSerial.ProcessBins_RING(x1);
            ////await Task.Delay(10);
            //FrequencySplitterSerial.ProcessBins_RING(x2);
            ////await Task.Delay(10);
            //FrequencySplitterSerial.ProcessBins_RING(x3);
            ////await Task.Delay(10);
            //FrequencySplitterSerial.ProcessBins_RING(x4);
            ////await Task.Delay(10);
            //FrequencySplitterSerial.ProcessBins_RING(x5);
            //FrequencySplitterSerial.ProcessBins_RING(x6);
            //FrequencySplitterSerial.ProcessBins_RING(x7);
            //FrequencySplitterSerial.ProcessBins_RING(x8);
            //FrequencySplitterSerial.ProcessBins_RING(v1.Select(x => x.Magnitude).ToArray());
            //await Task.Delay(16);
            //FrequencySplitterSerial.ProcessBins_RING(v2.Select(x => x.Magnitude).ToArray());
            //await Task.Delay(16);
            //FrequencySplitterSerial.ProcessBins_RING(v3.Select(x => x.Magnitude).ToArray());
            //await Task.Delay(16);
            //FrequencySplitterSerial.ProcessBins_RING(v4.Select(x => x.Magnitude).ToArray());
            //await Task.Delay(10);
            //FrequencySplitterSerial.ProcessBins_RING(values.Select(x => x.Magnitude).ToArray());
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

            FrequencySplitter.ProcessBins_RING(toSend);
        }
    }
}

