# Part of Matlab to Kodiak interface connecting to the Kodiak part.
# Read messages passed from matlab and passes them to Kodiak.
# Afterwards writes back the answer to the shared file.
# All the string read from the shared file need to be encoded in UTF-8
from __future__ import print_function
import kodiakmessages_pb2
import sys
import kodiak_parser
from interface import Interface
import time

def ReadBifurcation(bifurcations):
  '''Reads the bifurcation message stored in bifurcations.

     Args:
          bifurcations: A bifurcations list containing bifurcation from kodiakmessages.proto.

     Returns:
          A tuple containing Boolean and a list.
          (True, List) if there was a bifurcation written the file.
          (False, ()) if not.
  '''
  # If no bifurcation problems written to file return.
  if(len(bifurcations) is None):
    return (False,())
  interface = Interface()
  parser = kodiak_parser.KodiakParser(interface = interface)
  parser.parse("set safe input = false")
  for bifurcation in bifurcations:
    for var in bifurcation.var:
      # Passes variables from shared file to kodiak
      interface.var_exp(var.name.encode("UTF-8"),('INTERVAL', ("approx",var.lb), ("approx",var.ub)))
    for param in bifurcation.param:
      # Passes params from shared file to kodiak
      interface.var_exp(param.name.encode("UTF-8"),('INTERVAL', ("approx",param.lb ), ("approx",param.ub)), False)
    
    for dfeq in bifurcation.dfeq:
      # Passes csntrs from shared file to parser which then sends it to kodiak
      parser.parse("dfeq "+dfeq.dfeq.encode("UTF-8"))

    for cnstr in bifurcation.cnstr:
      # Passes csntrs from shared file to parser which then sends it to kodiak
      parser.parse("cnstr "+cnstr.cnstr.encode("UTF-8"))
    # Tells kodiak to solve a bifurcation problem.  
    interface.bifurcation()
    # Extracts the answer from kodiak into boxes prepared for writing to file
    boxes = interface.prepareboxes()
    interface.reset_env()
    return (True, boxes)

def WriteBifurcationAns(answer, boxes):
  '''Writes the answer produced by Kodiak to shared file.

     Args:
          answer: a bif_ans from kodiakmessages.proto
          boxes: boxes to be written to file.

  '''
  for box_type in boxes:
    boxt = answer.boxtype.add()
    for box in box_type:
      b = boxt.box.add()
      for dim in box:
        i = b.interval.add()
        i.lb = dim[0]
        i.ub = dim[1]



# Main procedure:  Reads all the kodiak_messages from a file,
# If there are new problems defined solves them and writes back the answer
# Else sleeps for 1s  
if(__name__ == "__main__"):
  print("\nServer for kodiak-matlab interface started...")
  while(True):
    kodiak_messages = kodiakmessages_pb2.KodiakMessages()
    f = open("kodiak_messages", "rb")
    kodiak_messages.ParseFromString(f.read())
    f.close()
    if(kodiak_messages.m_done): 
      print("Reading message and calculating answer...")
      ans, boxes = ReadBifurcation(kodiak_messages.bifurcation)
      if(ans):
        WriteBifurcationAns(kodiak_messages.bifans.add(), boxes)
      kodiak_messages.m_done = False
      kodiak_messages.k_done = True
      f = open("kodiak_messages", "wb")
      f.write(kodiak_messages.SerializeToString())
      f.close()
      print("Answer written back...")
    else:
      time.sleep(1)