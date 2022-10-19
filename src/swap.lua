t = {}

function fill(l) 
	for i = 1, l do
		t[i] = i
	end
end

function out(...)
	for i = 1, #t  do
		io.write(tostring(t[i]), ' ');
	end
	print(';', ...)
end

function move(d)
	local d = d

	for i = 1, #t + ((#t + 1) % d) do
		local c = math.min(math.max(1, i - d), #t)
		local ii = math.min(#t, i)
		t[c], t[ii] = t[ii], t[c]
		out((i) % d, (#t - 1) % d)
	end
end

fill(tonumber(arg[1]))
move(tonumber(arg[2]), arg[3])
