
function Body( object )
	coroutine.yield();
	local graphics = Graphics;
	local transform = pointer.Transform();
	local pos = pointer.Chunk();
	
	object:Transform(transform);
	transform:Position(pos);

	pos:SetF( 0.01, 0.9, 0.0 );
	
	local text = pointer.Text();
	local str = pointer.Chunk();
	
	object:Text(text);
	text:Content(str);

	while true do
		str:SetS( string.format("%.3f",blackboard.dt), 10 );
		coroutine.yield();
	end
end
