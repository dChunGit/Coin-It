package com.simplex.coin2credit

interface TransactionInterface {
    fun relayMessage(data: String)
    fun messageWritten(success: Int)
    fun databaseRead(transactions: List<Transaction>)
}