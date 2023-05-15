wrk -d 30 -t 1 -c 1 --latency -s ./get_cache.lua http://localhost:8082/
wrk -d 30 -t 1 -c 10 --latency -s ./get_cache.lua http://localhost:8082/
wrk -d 30 -t 1 -c 100 --latency -s ./get_cache.lua http://localhost:8082/
wrk -d 30 -t 5 -c 10 --latency -s ./get_cache.lua http://localhost:8082/
wrk -d 30 -t 5 -c 100 --latency -s ./get_cache.lua http://localhost:8082/
wrk -d 30 -t 10 -c 10 --latency -s ./get_cache.lua http://localhost:8082/
wrk -d 30 -t 10 -c 100 --latency -s ./get_cache.lua http://localhost:8082/

echo "no_cache"

wrk -d 30 -t 1 -c 1 --latency -s ./get_no_cache.lua http://localhost:8082/
wrk -d 30 -t 1 -c 10 --latency -s ./get_no_cache.lua http://localhost:8082/
wrk -d 30 -t 1 -c 100 --latency -s ./get_no_cache.lua http://localhost:8082/
wrk -d 30 -t 5 -c 10 --latency -s ./get_no_cache.lua http://localhost:8082/
wrk -d 30 -t 5 -c 100 --latency -s ./get_no_cache.lua http://localhost:8082/
wrk -d 30 -t 10 -c 10 --latency -s ./get_no_cache.lua http://localhost:8082/
wrk -d 30 -t 10 -c 100 --latency -s ./get_no_cache.lua http://localhost:8082/