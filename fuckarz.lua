function main()
   while not isSampAvailable() do wait(0) end
   while true do
      wait(0)
      if sampIsDialogActive() and sampGetCurrentDialogId() == 26190 then
         wait(1000)
         sampSendDialogResponse(26190, 1, -1, "")
      end
   end
end