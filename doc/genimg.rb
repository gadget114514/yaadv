require 'csv'

colors=[]
f = File.read("color.txt")
f.each_line do |col|
	col.chomp!
	colors.push(col)
end


file = CSV.read("uiimg_size_spec.txt", col_sep:" ", headers:false) ;
#	puts file

i = 0;	
	file.each do |line|
		fn = line[0]
		sz = line[1]
		color = colors[i % colors.length]
		a = fn;
		b = a.gsub(/^yaadv\//, "")
		cmd ="convert -size #{sz} xc:#{color} -pointsize 12 -draw \"text 0,14 #{b}\" gen/#{fn}"
		puts cmd

		i = i + 1;
	end
