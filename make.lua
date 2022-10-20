local shell     = function(prog, ...)
	local r = os.execute(table.concat({prog, ...}));
	if not r then
		error('command '..prog..' fails to work!');
	end
end

local make_root = shell("pwd")
