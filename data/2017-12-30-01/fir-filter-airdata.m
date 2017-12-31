echo off

pkg load signal
graphics_toolkit('fltk')

% Data is sampled at 20 Hz
x = csvread('plot.csv')

time = transpose(1:1:length(x)) / 20

len = 100
istart = 23400
iend = istart + len

for orderindex = 1:4
  for freqindex = 1:4
    % Note frequency is in units of (pi radians/sample). It ranges from 0 to 1,
    % where a frequency of 1 means the Nyquist limit of the data, or a frequency
    % equal to 1/2 the sampling rate. In our case, that frequency is 10 Hz.
    order = orderindex * 5
    freq = freqindex * 0.2
    filtered = filter(fir1(order, freq), 1, x(:, 3))
    subplot(4, 4, (1 + (orderindex -1) * 4) + (freqindex - 1))
    plot(...
     time(istart:iend) - time(istart), ...
      filtered(istart:iend), ...
      time(istart:iend) - time(istart), ...
      x(istart:iend, 3))
    title(sprintf("N = %d    f = %1.2f Hz", order, freq * 10))
    set(gca, "fontsize", 8);
    xlabel("time (s)")
    ylabel("Q (Pa)")
  end
end

print('fir-filter-airdata.png','-dpng','-r1200');
