yangxu-All-Series# ./ab  -n 100000 -c 1000 http://127.0.0.1:12345/favicon.ico 
This is ApacheBench, Version 2.3 <$Revision: 1843412 $>
Copyright 1996 Adam Twiss, Zeus Technology Ltd, http://www.zeustech.net/
Licensed to The Apache Software Foundation, http://www.apache.org/

Benchmarking 127.0.0.1 (be patient)
Completed 10000 requests
Completed 20000 requests
Completed 30000 requests
Completed 40000 requests
Completed 50000 requests
Completed 60000 requests
Completed 70000 requests
Completed 80000 requests
Completed 90000 requests
Completed 100000 requests
Finished 100000 requests


Server Software:        
Server Hostname:        127.0.0.1
Server Port:            12345

Document Path:          /favicon.ico
Document Length:        555 bytes

Concurrency Level:      1000
Time taken for tests:   4.623 seconds
Complete requests:      100000
Failed requests:        46
   (Connect: 0, Receive: 0, Length: 23, Exceptions: 23)
Total transferred:      61785786 bytes
HTML transferred:       55487235 bytes
Requests per second:    21633.06 [#/sec] (mean)
Time per request:       46.226 [ms] (mean)
Time per request:       0.046 [ms] (mean, across all concurrent requests)
Transfer rate:          13052.89 [Kbytes/sec] received

Connection Times (ms)
              min  mean[+/-sd] median   max
Connect:        0   31 135.9     13    1054
Processing:     1   14  18.2     14     433
Waiting:        0   10  17.0      9     430
Total:          2   45 144.6     33    1461

Percentage of the requests served within a certain time (ms)
  50%     33
  66%     40
  75%     41
  80%     41
  90%     46
  95%     47
  98%     51
  99%   1048
 100%   1461 (longest request)