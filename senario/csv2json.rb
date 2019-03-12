require 'csv'
require 'json'

csv_data = CSV.read(ARGV[0], headers: true)

a = [];

  csv_data.each do |data|
#	a.push(data);
#	p (data);
	x = {};
	data.each{|key,val| if (key) then x[key] = val; end }
	a.push(x);
  end

str = JSON.dump(a);
puts str;


