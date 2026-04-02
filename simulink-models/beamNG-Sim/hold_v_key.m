function hold_v_key()
import java.awt.Robot;
import java.awt.event.*;

robot = Robot;
robot.keyPress(KeyEvent.VK_V);
pause(2);   % hold duration
robot.keyRelease(KeyEvent.VK_V);
end