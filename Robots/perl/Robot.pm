

require Exporter;
@ISA = qw(Exporter);

@EXPORT =qw(
  UNKNOWN_MESSAGE
  PROCESS_TIME_LOW
  MESSAGE_SENT_IN_ILLEGAL_STATE
  UNKNOWN_OPTION
  OBSOLETE_KEYWORD

  ROBOT_MAX_ROTATE
  ROBOT_CANNON_MAX_ROTATE
  ROBOT_RADAR_MAX_ROTATE
  ROBOT_MAX_ACCELERATION
  ROBOT_MIN_ACCELERATION
  ROBOT_START_ENERGY
  ROBOT_MAX_ENERGY
  ROBOT_ENERGY_LEVELS
  SHOT_SPEED
  SHOT_MIN_ENERGY
  SHOT_MAX_ENERGY
  SHOT_ENERGY_INCREASE_SPEED
  TIMEOUT
  DEBUG_LEVEL

  SIGNAL
  SEND_SIGNAL
  SEND_ROTATION_REACHED

  NOOBJECT
  ROBOT
  SHOT
  WALL
  COOKIE
  MINE
  LAST_OBJECT_TYPE
);

%EXPORT_TAGS = (
  'warnings' => [qw(UNKNOWN_MESSAGE PROCESS_TIME_LOW
                    MESSAGE_SENT_IN_ILLEGAL_STATE
		    UNKNOWN_OPTION OBSOLETE_KEYWORD
		)],
  'options'  => [qw(ROBOT_MAX_ROTATE ROBOT_CANNON_MAX_ROTATE
                    ROBOT_RADAR_MAX_ROTATE ROBOT_MAX_ACCELERATION
		    ROBOT_MIN_ACCELERATION ROBOT_START_ENERGY
		    ROBOT_MAX_ENERGY ROBOT_ENERGY_LEVELS
		    SHOT_SPEED SHOT_MIN_ENERGY SHOT_MAX_ENERGY
		    SHOT_ENERGY_INCREASE_SPEED TIMEOUT
		    DEBUG_LEVEL
                )],
  'robot_options' => [qw(SIGNAL SEND_SIGNAL SEND_ROTATION_REACHED)],
  'objects'       => [qw(NOOBJECT ROBOT SHOT WALL COOKIE MINE
  		         LAST_OBJECT_TYPE
                     )]
);

BEGIN{
  sub UNKNOWN_MESSAGE()               	{0}
  sub PROCESS_TIME_LOW()              	{1}
  sub MESSAGE_SENT_IN_ILLEGAL_STATE() 	{2}
  sub UNKNOWN_OPTION()                	{3}
  sub OBSOLETE_KEYWORD()              	{4}

  sub ROBOT_MAX_ROTATE()		{0}
  sub ROBOT_CANNON_MAX_ROTATE()		{1}
  sub ROBOT_RADAR_MAX_ROTATE()		{2}
  sub ROBOT_MAX_ACCELERATION()		{3}
  sub ROBOT_MIN_ACCELERATION()		{4}
  sub ROBOT_START_ENERGY()		{5}
  sub ROBOT_MAX_ENERGY()		{6}
  sub ROBOT_ENERGY_LEVELS()		{7}
  sub SHOT_SPEED()			{8}
  sub SHOT_MIN_ENERGY()			{9}
  sub SHOT_MAX_ENERGY()			{10}
  sub SHOT_ENERGY_INCREASE_SPEED()	{11}
  sub TIMEOUT()				{12}
  sub DEBUG_LEVEL()			{13}

  sub SIGNAL()				{2}
  sub SEND_SIGNAL()			{0}
  sub SEND_ROTATION_REACHED()		{1}

  sub NOOBJECT()			{-1}
  sub ROBOT()				{0}
  sub SHOT()				{1}
  sub WALL()				{2}
  sub COOKIE()				{3}
  sub MINE()				{4}
  sub LAST_OBJECT_TYPE()		{5}
}

#*MAX_BRAKE = \MAX_BRAKE();

##################################################################
# CMDs
sub RobotOption($$){
  my ($nr, $value) = @_;
  print "RobotOption $nr $value\n";
}

sub Name($) {
  my $name = shift;
  print "Name $name\n";
}

sub Colour($) {
  my $color = shift;
  print "Colour $color\n";
}
sub Color($) { Colour(@_) }

sub Rotate($$) {
  my $what = shift; # 1 - robot, 2 - cannon, 4 - radar
  my $v    = shift; # velocity
  print "Rotate $what $v\n";
}

sub RotateTo($$$) {
  my ($what, $v, $end) = @_;
  print "RotateTo $what $v $end\n";
}

sub RotateAmount($$$) {
  my ($what, $v, $angl) = @_;
  print "RotateAmount $what $v $angl\n";
}

sub Sweep($$$$) {
  my ($what, $v, $la, $ra) = @_;
  print "Sweep $what $v $la $ra\n";
}

sub Accelerate($) {
  my $value = shift;
  print "Accelerate $value\n";
}

sub Brake($) {
  my $portion = shift;
  print "Brake $portion\n";
}

sub Shoot($) {
  my $energy = shift;
  print "Shoot $energy\n";
}

sub Print(@) {
  my $txt = join(" ",@_);
  print "Print $txt\n";
}

sub Debug($) {
  my $txt = shift;
  print "Debug $txt\n";
}

sub DebugLine($$$$) {
  my ($a1, $r1, $a2, $r2) = @_;
  print "DebugLine $a1 $r1 $a2 $r2\n";
}

sub DebugCircle($$$) {
  my ($a,$r1,$r2) = @_;
  print "DebugCircle $a $r1 $r2\n";
}

1;
