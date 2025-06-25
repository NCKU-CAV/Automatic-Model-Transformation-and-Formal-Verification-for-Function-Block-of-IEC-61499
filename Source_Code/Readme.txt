Method for creating a txt file​:

Before discussing how we perform model transformation, it is necessary to standardize the input models, which primarily involves the components within each function block, as described above in the function block definition. The input model is mainly composed of individual function blocks, and designers can use the transformed models of these blocks to assemble the desired system, which can then be verified. The reason for not including the transformation of the entire system is that the connection mechanisms between function blocks are not specified by the IEC 61499 standard. These aspects are left to the designers, who must tailor the invocation of function blocks according to the specific behavior of each system. However, the behavior of an individual function block can be standardized. Therefore, our focus here is on the model transformation of each function block within the system.

1.Interface:

First, we introduce the interface part of the function block.
We begin by specifying the types of variables. In the function block interface, there are four different types of inputs and outputs: input event signals (labeled as input event in the diagram), output event signals (output event), input data signals (input data), and output data signals (output data).
Next, the names of the variables are entered accordingly.

According to the standard, event signals are defined as Boolean values, meaning they only represent true or false outputs. However, for data signals, their data types are not specified by the standard. Therefore, the data type of each variable must be explicitly declared after the variable name.
For example, if the variable vi1 is of integer type and its range is from 0 to 10, we express its data type by appending “0..10” after vi1.

Ex.

/////////////////////////////////////////
input_event ei1 ei2;

output_event eo1 eo2;

input_data
vi1 0..10
vi2 0..10;

output_data
vo1 0..20
vo2 0..20;
/////////////////////////////////////////


2.ECC:

First, the states of the ECC (Execution Control Chart) are entered. In this case, there are three states, so the names of all states are listed after the keyword “state.”
Next, the transitions of the ECC are described. Each transition consists of a source state, a transition condition, and a destination state. For example, if the condition for transitioning from state q0 to state q1 is ei1, then the transition should be written as “q0 ei1 q1”.

In the ECC, when multiple transitions share the same source state, a priority order is applied to determine which transition is executed first. In the input model, transitions with higher priority are listed above those with lower priority.

Take Figure 4 as an example:

/////////////////////////////////////////
ECC

state q0 q1 q2;

transition(
q0 ei1 q1
q0 ei2&(vi2=1) q2
q1 vi1=1 q0
q2 vi2=1 q0
)
////////////////////////////////////////

In the above case, there are two transitions from the source state q0. Since the transition (q0, ei1, q1) has a higher priority than (q0, ei2 ∧ (vi2 = 1), q2), we list (q0, ei1, q1) above the other.


3.Sub-behaviors as Finite State Machine Models:

After specifying the function block’s interface and the ECC, the designer must input the operational behavior for each state. Since the algorithms within the operational behaviors can be implemented in many different ways, designers are required to first convert them into a unified finite state machine (FSM) model using the method we provide.

Generally, the internal operational behavior consists of multiple sub-behaviors executed sequentially, as defined by the user. Therefore, our main focus here is on how to model these various sub-behaviors. There are typically three types of sub-behaviors within an operational behavior: (1) a sub-behavior consisting of an event output, (2) a sub-behavior consisting of an algorithm, and (3) a sub-behavior consisting of both an algorithm and an event output. The sub-behaviors originally described in the ECC are first converted into FSM models. Again, take Figure 4 as an example:

////////////////////////////////////////
action (
	q0{
	}

	q1{
		a1 [
			state e1 done;
			transition (
				e1 TRUE / vo1 = vi1 + vi2 eo1=TRUE done;
			)
		]
	}
	 
	q2{
		a1 [
			state e1 done;
			transition (
				e1 TRUE / vo2 = vi2 eo2=TRUE done;
			)
	}
////////////////////////////////////////

In the example above, you can see that in state q1, the operational behaviors required for that state are specified. Here, a1 refers to an internal sub-behavior, executed in the order defined (top to bottom). The sub-behaviors originally associated with state q1 can be converted into an FSM model. When entering this model for the corresponding sub-behavior, it is mainly divided into states and transitions. There are two states: e1 and done. The initial state, e1, is written first, and the final state, done, is written last. For the transitions, enter the source state, condition, action, and destination state in sequence. In this case, it is entered as “e1 TRUE / vo1=vi1+vi2 eo1=TRUE done,” where the actions to be performed are specified after the “/”.

Using this method, each sub-behavior within a state is converted into our required input format. With this unified sub-behavior model, we can facilitate the overall model transformation of the function block.

Additionally, if a state does not have any operational behavior to execute, simply input a single state to indicate that there is no operational behavior. For example, if state q0 does not require any operational behavior, its sub-behavior model will only have a single state and no transitions. If no action is specified, a single state is automatically generated. Therefore, in Figure 3.14, there is no need to specify any actions after q0.



**************************************************************************************************



Execution method:

1. Download NuSMV:
Visit the following website:
https://nusmv.fbk.eu/downloads.html
Download version 2.6.0 from the Binaries section according to your operating system.
After the download is complete, verify that the bin directory inside the NuSMV folder contains the NuSMV executable file.
Then, copy all contents from the FB2SMV folder into the bin directory of NuSMV, so that you can use NuSMV for subsequent verification.

2.Generate the FA model (.smv file):
In FB2SMV.cpp, you can modify the input .txt filename and the output .smv filename within the main function by editing the ifstream and ofstream statements (lines 108–110). Then, compile the program using the command:'g++ FB2SMV.cpp -o FB2SMV'. This will generate the executable FB2SMV.exe. Running FB2SMV.exe will then generate the corresponding .smv file based on the specified input filename.

3.Verify the correctness of FA behavior:
The property to be verified (as described in the paper) should be placed in a file named filename_property.smv. Copy the content of filename_property.smv to the bottom of the corresponding .smv model file, taking care not to overwrite the generated FA model.
For example:
Copy the contents of traffic_light_property.smv to the end of traffic_light.smv, then run the following command in the terminal:'.\NuSMV.exe traffic_light.smv'. The verification result for the traffic_light model will be returned.