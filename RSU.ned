package ned;

simple RSU //simple module named RSU
{
    parameters:
        int rsuId;// each RSU has an ID
    gates:
        input fromEV; //Gate that receives messages from EVs
        output toController; //Gate that sends messages to the Controller
}
