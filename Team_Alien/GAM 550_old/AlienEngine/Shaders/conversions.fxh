inline bool Flagged( uint flag, uint option )
{
	if ( ( flag & option ) == option ) return true;
	return false;
}