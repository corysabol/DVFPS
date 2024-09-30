let dockerfile_name = "Dockerfile.dev"
let image_name = "dvfps-server-dev"

echo "Building Docker image with $dockerfile_name..."
docker build -f $dockerfile_name -t $image_name .

echo "Running build command inside Docker container..."
docker run --rm -v $"($nu.path):/usr/src/dev" -w /usr/src/dev $image_name nu -c 'mkdir build && cd build && cmake ..'
