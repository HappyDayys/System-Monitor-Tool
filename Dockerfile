# Dockerfile
FROM ubuntu:24.04

WORKDIR /app

# Install g++ (and optionally less if you want)
RUN apt-get update && apt-get install -y g++ build-essential && rm -rf /var/lib/apt/lists/*

# Copy source
COPY main.cpp .

# Compile
RUN g++ main.cpp -o system_monitor

# Run the monitor program (allow passing optional interval)
CMD ["./system_monitor"]
