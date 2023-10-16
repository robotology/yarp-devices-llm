import yarp
import sys

class Module(yarp.RFModule):

    def configure(self, rf):
        
        # Opening the device
        prop = yarp.Property()
        prop.put("device","GPTDevice")
        self.poly = yarp.PolyDriver()
        self.poly.open(prop)
        self.illm_ = self.poly.viewILLM()

        # Opening the streaming port
        chat_port_name = "/chat/text:i"
        self.local_port = yarp.BufferedPortBottle()
        self.local_port.open(chat_port_name)

        return True

    def updateModule(self):
        
        # Asking the question
        question = self.local_port.read().get(0).asString()
        answer = "ciao"
        self.illm_.setPrompt(answer)
        self.illm_.readPrompt(answer)
        print(answer)
        return True
    
    def close(self):
        self.poly.close()
        self.local_port.close()
        return True
    
    def interruptModule(self):
        self.poly.close()
        self.local_port.interrupt()
        return True
    
    def getPeriod(self):
        return 0.02


def main():
    yarp.Network.init()

    rf = yarp.ResourceFinder()
    rf.configure(sys.argv)

    module = Module()
    
    if(module.configure(rf)):
        module.runModule()

if __name__ == '__main__':
    main()