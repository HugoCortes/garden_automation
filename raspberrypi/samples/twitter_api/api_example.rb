require 'rubygems'
require 'oauth'
require 'json'

# Keys from dev.twitter.com
# The consumer key identifies the application making the request.
# The access token identifies the user making the request.
consumer_key = OAuth::Consumer.new(
    # Consumer Key
    "HERE",
    # Consumer Secret
    "HERE")
access_token = OAuth::Token.new(
    # Access Token
    "HERE",
    # Access Token Secret
    "HERE")


baseurl = "https://api.twitter.com"
path    = "/1.1/statuses/update.json"
address = URI("#{baseurl}#{path}")
request = Net::HTTP::Post.new address.request_uri
request.set_form_data(
  "status" => "Hello World!",
)

# Set up HTTP.
http             = Net::HTTP.new address.host, address.port
http.use_ssl     = true
http.verify_mode = OpenSSL::SSL::VERIFY_PEER

# Issue the request.
request.oauth! http, consumer_key, access_token
http.start
response = http.request request

# Parse and print the Tweet if the response code was 200
tweet = nil
if response.code == '200' then
  tweet = JSON.parse(response.body)
  puts "Successfully sent #{tweet["text"]}"
else
  puts "Could not send the Tweet! " +
  "Code:#{response.code} Body:#{response.body}"
end
