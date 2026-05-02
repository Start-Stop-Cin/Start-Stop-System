function press_v_key()
import java.awt.Robot;
import java.awt.event.*;

robot = Robot;
robot.keyPress(KeyEvent.VK_V);
robot.keyRelease(KeyEvent.VK_V);
end